//////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
// otserv main. The only place where things get instantiated.
//////////////////////////////////////////////////////////////////////
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//////////////////////////////////////////////////////////////////////
#include "otpch.h"

#include "definitions.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <stdlib.h>
#include <time.h>

#include "otsystem.h"
#include "server.h"

#include "game.h"
#include "status.h"
#include "monsters.h"
#include "ioplayer.h"
#include "npc.h"
#include "commands.h"
#include "vocation.h"
#include "scriptmanager.h"
#include "configmanager.h"

#include "tools.h"
#include "ban.h"

#ifdef __OTSERV_ALLOCATOR__
#include "allocator.h"
#endif

#ifdef BOOST_NO_EXCEPTIONS
	#include <exception>
	void boost::throw_exception(std::exception const & e){
		std::cout << "Boost exception: " << e.what() << std::endl;
	}
#endif

IPList serverIPs;

ConfigManager g_config;

Game g_game;
Commands commands;
Npcs g_npcs;
Monsters g_monsters;
BanManager g_bans;
Vocations g_vocations;
Server* g_server = NULL;

boost::mutex g_loaderLock;
boost::condition_variable g_loaderSignal;
boost::unique_lock<boost::mutex> g_loaderUniqueLock(g_loaderLock);

#if defined __EXCEPTION_TRACER__
#include "exception.h"
#endif
#include "networkmessage.h"

#if !defined(__WINDOWS__)
time_t start_time;
#endif

void ErrorMessage(const char* message) {
	std::cout << std::endl << std::endl << "Error: " << message << std::endl;

	std::string s;
	std::cin >> s;
}

void ErrorMessage(std::string m){
	ErrorMessage(m.c_str());
}

struct CommandLineOptions{
	std::string configfile;
	bool truncate_log;
	std::string logfile;
	std::string errfile;
	std::string runfile;
};

CommandLineOptions g_command_opts;

bool parseCommandLine(CommandLineOptions& opts, std::vector<std::string> args);
void mainLoader(const CommandLineOptions& command_opts);

#if !defined(__WINDOWS__)
// Runfile, for running OT as daemon in the background. If the server is shutdown by internal
// means, we need to clear the file to notify the daemon manager of our change in status.
// Note that if the server crashes, this will not happend. :|
void closeRunfile(void)
{
	std::ofstream runfile(g_command_opts.runfile.c_str(), std::ios::trunc | std::ios::out);
	runfile.close(); // Truncate file
}
#endif

int main(int argc, char *argv[])
{
	if(parseCommandLine(g_command_opts, std::vector<std::string>(argv, argv + argc)) == false){
		return 0;
	}

	#if !defined(__WINDOWS__)
	if(g_command_opts.runfile != ""){
		std::ofstream f(g_command_opts.runfile.c_str(), std::ios::trunc | std::ios::out);
		f << getpid();
		f.close();
		atexit(closeRunfile);
	}
	#endif

	boost::shared_ptr<std::ofstream> logfile;
	boost::shared_ptr<std::ofstream> errfile;
	if(g_command_opts.logfile != ""){
		logfile.reset(new std::ofstream(g_command_opts.logfile.c_str(),
			(g_command_opts.truncate_log? std::ios::trunc : std::ios::app) | std::ios::out)
		);
		if(!logfile->is_open()){
			ErrorMessage("Could not open standard log file for writing!");
		}
		std::cout.rdbuf(logfile->rdbuf());
	}
	if(g_command_opts.errfile != ""){
		errfile.reset(new std::ofstream(g_command_opts.errfile.c_str(),
			(g_command_opts.truncate_log? std::ios::trunc : std::ios::app) | std::ios::out)
		);
		if(!errfile->is_open()){
			ErrorMessage("Could not open error log file for writing!");
		}
		std::cerr.rdbuf(errfile->rdbuf());
	}

#if !defined(__WINDOWS__)
	// TODO: find something better than this hack. :|
	time(&start_time);
#endif

#ifdef __OTSERV_ALLOCATOR_STATS__
	boost::thread(boost::bind(&allocatorStatsThread, (void*)NULL));
#endif

#if defined __EXCEPTION_TRACER__
	ExceptionHandler mainExceptionHandler;
	mainExceptionHandler.InstallHandler();
#endif

	std::cout << ":: " OTSERV_NAME " Version " OTSERV_VERSION << std::endl;
	std::cout << ":: by Rodrigo (Nottinghster)" << std::endl;
	std::cout << ":: Compilation date: " __DATE__ " " __TIME__ << std::endl;
	std::cout << ":: ==============================================" << std::endl;
	std::cout << "::" << std::endl;

#if defined __DEBUG__MOVESYS__ || defined __DEBUG_HOUSES__ || defined __DEBUG_MAILBOX__ \
	|| defined __DEBUG_LUASCRIPTS__ || defined __DEBUG_RAID__ || defined __DEBUG_NET__ \
    || defined __DEBUG_SQL__

	std::cout << ":: Debugging:";
	#ifdef __DEBUG__MOVESYS__
	std::cout << " MOVESYS";
	#endif
	#ifdef __DEBUG_MAILBOX__
	std::cout << " MAILBOX";
	#endif
	#ifdef __DEBUG_HOUSES__
	std::cout << " HOUSES";
	#endif
	#ifdef __DEBUG_LUASCRIPTS__
	std::cout << " LUA-SCRIPTS";
	#endif
	#ifdef __DEBUG_RAID__
	std::cout << " RAIDS";
	#endif
	#ifdef __DEBUG_NET__
	std::cout << " NET-ASIO";
	#endif
	#ifdef __DEBUG_SQL__
	std::cout << " SQL";
	#endif
	std::cout << std::endl;
#endif

#if !defined(WIN32) && !defined(__ROOT_PERMISSION__)
	if( getuid() == 0 || geteuid() == 0 ){
		std::cout << std::endl << "OTServ executed as root user, please login with a normal user." << std::endl;
		return 1;
	}
#endif


#if defined __WINDOWS__ || defined WIN32
	//nothing yet
#else
	// ignore sigpipe...
	struct sigaction sigh;
	sigh.sa_handler = SIG_IGN;
	sigh.sa_flags = 0;
	sigemptyset(&sigh.sa_mask);
	sigaction(SIGPIPE, &sigh, NULL);
#endif
    
    Dispatcher::getDispatcher().addTask(createTask(boost::bind(mainLoader, g_command_opts)));
    
    g_loaderSignal.wait(g_loaderUniqueLock);
    
    int port = g_config.getNumber(ConfigManager::PORT);

	Server server(INADDR_ANY, port);
	std::cout << "[done]" << std::endl << ":: Tibia World RPG OldSchool Server Running..." << std::endl;
	g_server = &server;
	server.run();
    
#if defined __EXCEPTION_TRACER__
    mainExceptionHandler.RemoveHandler();
#endif
}

bool parseCommandLine(CommandLineOptions& opts, std::vector<std::string> args)
{
	std::vector<std::string>::iterator argi = args.begin();
	opts.truncate_log = false;

	if(argi != args.end()){
		++argi;
	}

	while(argi != args.end()){
		std::string arg = *argi;
		if(arg == "-p" || arg == "--port"){
			if(++argi == args.end()){
				std::cout << "Missing parameter for '" << arg << "'" << std::endl;
				return false;
			}
			g_config.setNumber(ConfigManager::PORT, atoi(argi->c_str()));
		}
		else if(arg == "-r" || arg == "--runfile"){
			if(++argi == args.end()){
				std::cout << "Missing parameter for '" << arg << "'" << std::endl;
				return false;
			}
			opts.runfile = *argi;
		}
		else if(arg == "-i" || arg == "--ip"){
			if(++argi == args.end()){
				std::cout << "Missing parameter for '" << arg << "'" << std::endl;
				return false;
			}
			g_config.setString(ConfigManager::IP, *argi);
		}
		else if(arg == "-c" || arg == "--config"){
			if(++argi == args.end()){
				std::cout << "Missing parameter for '" << arg << "'" << std::endl;
				return false;
			}
			opts.configfile = *argi;
		}
		else if(arg == "--truncate-log"){
			opts.truncate_log = true;
		}
		else if(arg == "-l" || arg == "--log-file"){
			if(++argi == args.end()){
				std::cout << "Missing parameter 1 for '" << arg << "'" << std::endl;
				return false;
			}
			opts.logfile = *argi;
			if(++argi == args.end()){
				std::cout << "Missing parameter 2 for '" << arg << "'" << std::endl;
				return false;
			}
			opts.errfile = *argi;
		}
		else if(arg == "--help"){
			std::cout <<
"Usage: otserv {-i|-p|-c|-r|-l}\n"
"\n"
"\t-i, --ip $1\t\tIP of gameworld server. Should be equal to the \n"
"\t\t\t\tglobal IP.\n"
"\t-p, --port $1\t\tPort for server to listen on.\n"
"\t-c, --config $1\t\tAlternate config file path.\n"
"\t-l, --log-file $1 $2\tAll standard output will be logged to the\n"
"\t\t\t\t$1 file, all errors will be logged to $2.\n"
"\t-r, --run-file $1\t\tSpecifies a runfile. Will contain the pid\n"
"\t\t\t\tof the server process as long as it is running (UNIX).\n"
"\t--truncate-log\t\tReset log file each time the server is \n"
"\t\t\t\tstarted.\n";
			return false;
		}
		else
		{
			std::cout << "Unrecognized command line argument '" << arg << "'\n"
			"Usage: otserv {-i|-p|-c|-r|-l}" << "\n";
			return false;
		}
		++argi;
	}
	return true;
}

void mainLoader(const CommandLineOptions& command_opts)
{
	//dispatcher thread
	g_game.setGameState(GAME_STATE_STARTUP);

	// random numbers generator
	std::cout << ":: Initializing the random numbers... " << std::flush;
	srand((unsigned int)OTSYS_TIME());
	std::cout << "[done]" << std::endl;

#if defined LUA_CONFIGFILE
	const char* configname = LUA_CONFIGFILE;
#elif defined __LUA_NAME_ALTER__
	const char* configname = "otserv.lua";
#else
	const char* configname = "config.lua";
#endif

	if(command_opts.configfile != ""){
		configname = command_opts.configfile.c_str();
	}

	// read global config
	std::cout << ":: Loading lua script " << configname << "... " << std::flush;
#if !defined(WIN32) && !defined(__NO_HOMEDIR_CONF__)
	std::string configpath;
	configpath = getenv("HOME");
	configpath += "/.otserv/";
	configpath += configname;
	if (!g_config.loadFile(configname) && !g_config.loadFile(configpath))
#else
	if (!g_config.loadFile(configname))
#endif
	{
		std::ostringstream os;
#if !defined(WIN32) && !defined(__NO_HOMEDIR_CONF__)
		os << "Unable to load " << configname << " or " << configpath;
#else
		os << "Unable to load " << configname;
#endif
		ErrorMessage(os.str());
		exit(-1);
	}
	std::cout << "[done]" << std::endl;
	
#ifdef WIN32
	std::string serverPriorityName;
	std::string serverPriority = g_config.getString(ConfigManager::SERVER_PRIORITY);
	if(serverPriority == "realtime"){
	    serverPriorityName = "Real Time";
	    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    }
	else if(serverPriority == "high"){
	    serverPriorityName = "High";
	    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    }
	else if(serverPriority == "aboveNormal"){
	    serverPriorityName = "Above Normal";
	    SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
    }
    else {
        serverPriorityName = "Normal";
    }
	std::cout << ":: Server priority set to " << serverPriorityName << std::endl;	
#endif	

	std::cout << ":: Checking Database Connection... ";
	Database* db = Database::instance();
	if(db == NULL || !db->isConnected())
	{
		ErrorMessage("Database Connection Failed!");
		exit(-1);
	}
	std::cout << "[done]" << std::endl;

	std::stringstream filename;

	//load vocations
	filename.str("");
	filename << g_config.getString(ConfigManager::DATA_DIRECTORY) << "vocations.xml";
	std::cout << ":: Loading " << filename.str() << "... " << std::flush;
	if(!g_vocations.loadFromXml(g_config.getString(ConfigManager::DATA_DIRECTORY))){
		ErrorMessage("Unable to load vocations!");
		exit(-1);
	}
	std::cout << "[done]" << std::endl;

	//load commands
	filename.str("");
	filename << g_config.getString(ConfigManager::DATA_DIRECTORY) << "commands.xml";
	std::cout << ":: Loading " << filename.str() << "... " << std::flush;
	if(!commands.loadXml(g_config.getString(ConfigManager::DATA_DIRECTORY))){
		std::stringstream errormsg;
		errormsg << "Unable to load " << filename.str() << "!";
		ErrorMessage(errormsg.str().c_str());
		exit(-1);
	}
	std::cout << "[done]" << std::endl;

	// load item data
	filename.str("");
	filename << g_config.getString(ConfigManager::DATA_DIRECTORY) << "items/items.otb";
	std::cout << ":: Loading " << filename.str() << "... " << std::flush;
	if(Item::items.loadFromOtb(filename.str())){
		std::stringstream errormsg;
		errormsg << "Unable to load " << filename.str() << "!";
		ErrorMessage(errormsg.str().c_str());
		exit(-1);
	}
	std::cout << "[done]" << std::endl;

	filename.str("");
	filename << g_config.getString(ConfigManager::DATA_DIRECTORY) << "items/items.xml";
	std::cout << ":: Loading " << filename.str() << "... " << std::flush;
	if(!Item::items.loadFromXml(g_config.getString(ConfigManager::DATA_DIRECTORY))){
		std::stringstream errormsg;
		errormsg << "Unable to load " << filename.str() << "!";
		ErrorMessage(errormsg.str().c_str());
		exit(-1);
	}
	std::cout << "[done]" << std::endl;

	//load scripts
	if(ScriptingManager::getInstance()->loadScriptSystems() == false){
		exit(-1);
	}

	// load monster data
	filename.str("");
	filename << g_config.getString(ConfigManager::DATA_DIRECTORY) << "monsters/monsters.xml";
	std::cout << ":: Loading " << filename.str() << "... " << std::flush;
	if(!g_monsters.loadFromXml(g_config.getString(ConfigManager::DATA_DIRECTORY))){
		std::stringstream errormsg;
		errormsg << "Unable to load " << filename.str() << "!";
		ErrorMessage(errormsg.str().c_str());
		exit(-1);
	}
	std::cout << "[done]" << std::endl;

	//load experience stages
	filename.str("");
	filename << g_config.getString(ConfigManager::DATA_DIRECTORY) << "stages.xml";
	std::cout << ":: Loading stages.xml... ";
	if(!g_game.loadExperienceStages()){
		std::stringstream errormsg;
		errormsg << "Unable to load " << filename.str() << "!";
		ErrorMessage(errormsg.str().c_str());
		exit(-1);
	}
	std::cout << "[done]" << std::endl;
	
    //real tibia save system
    if(g_config.getBool(ConfigManager::ENABLE_SERVER_SAVE)){
        int32_t serverSaveHour = g_config.getNumber(ConfigManager::SERVER_SAVE_HOUR);
        int32_t hours;
        int32_t minutes;
        time_t getTime = time(NULL);
        const tm* timeValues = localtime(&getTime);

        if(timeValues->tm_hour > serverSaveHour)
        {
            hours = 24 - (timeValues->tm_hour - serverSaveHour);
            hours = hours - 1;
            minutes = 60 - timeValues->tm_min; 
        }
        else if(timeValues->tm_hour < serverSaveHour)
        {
            hours = (serverSaveHour - timeValues->tm_hour) - 1;
            minutes = 60 - timeValues->tm_min; 
        }
        else{
            hours = 23;
            minutes = 60 - timeValues->tm_min; 
        }
        
        uint32_t warnBefore = g_config.getNumber(ConfigManager::SAVE_WARN_BEFORE);
        uint32_t msToSave;
        uint32_t time = ((minutes * 60000) + ((hours * 60000) * 60)) - (timeValues->tm_sec * 1000);
        if(time < (warnBefore * 60000)){
            warnBefore = 60 - (timeValues->tm_min + 1);
            msToSave = 60000 - (timeValues->tm_sec * 1000);}
        else
            msToSave = (((minutes * 60000) + ((hours * 60000) * 60)) - (warnBefore * 60000)) - (timeValues->tm_sec * 1000);
        
        Scheduler::getScheduler().addEvent(createSchedulerTask(msToSave, 
            boost::bind(&Game::beforeServerSave, &g_game, warnBefore)));
            
        std::cout << ":: Server Save in " << hours << " hour(s), " << minutes << " minute(s)" << std::endl;
    }
    
    //simple save system
    int32_t simpleSaveFrequence = g_config.getNumber(ConfigManager::SIMPLE_SAVE_FREQUENCE);
    if(simpleSaveFrequence > 0){
        Scheduler::getScheduler().addEvent(createSchedulerTask((simpleSaveFrequence * 60000) - 500, 
            boost::bind(&Game::callSave, &g_game)));
    }
    
    //auto clean system
    int32_t autoCleanFrequence = g_config.getNumber(ConfigManager::AUTO_CLEAN_FREQUENCE);
    if(autoCleanFrequence > 0){
        int32_t cleanWarnBefore = g_config.getNumber(ConfigManager::CLEAN_WARN_BEFORE);
        if(cleanWarnBefore > 0 && cleanWarnBefore < autoCleanFrequence)
            Scheduler::getScheduler().addEvent(createSchedulerTask((autoCleanFrequence * 60000) - (cleanWarnBefore * 60000), boost::bind(&Game::cleanGameState, &g_game, cleanWarnBefore, true)));
        else
            Scheduler::getScheduler().addEvent(createSchedulerTask(autoCleanFrequence * 60000, boost::bind(&Game::cleanGameState, &g_game, 0, true)));
    }	

	std::string worldType = g_config.getString(ConfigManager::WORLD_TYPE);
	if(asLowerCaseString(worldType) == "pvp")
		g_game.setWorldType(WORLD_TYPE_PVP);
	else if(asLowerCaseString(worldType) == "no-pvp")
		g_game.setWorldType(WORLD_TYPE_NO_PVP);
	else if(asLowerCaseString(worldType) == "pvp-enforced")
		g_game.setWorldType(WORLD_TYPE_PVP_ENFORCED);
	else{
		ErrorMessage("Unknown world type!");
		exit(-1);
	}
	std::cout << ":: Worldtype: " << asUpperCaseString(worldType) << std::endl;
	
	std::cout << ":: Cleaning online players info... " << std::flush;
	if(!IOPlayer::instance()->cleanOnlineInfo()){
		std::stringstream errormsg;
		errormsg << "Unable to execute query for cleaning online status!";
		ErrorMessage(errormsg.str().c_str());
		exit(-1);
	}
	std::cout << "[done]" << std::endl;		

	#ifdef __SKULLSYSTEM__
	std::cout << ":: Skulls enabled" << std::endl;
	#endif

	std::string passwordType = g_config.getString(ConfigManager::PASSWORD_TYPE_STR);
	if(passwordType.empty() || asLowerCaseString(passwordType) == "plain"){
		g_config.setNumber(ConfigManager::PASSWORD_TYPE, PASSWORD_TYPE_PLAIN);
		std::cout << ":: Use plain passwords" << std::endl;
	}
	else if(asLowerCaseString(passwordType) == "md5"){
		g_config.setNumber(ConfigManager::PASSWORD_TYPE, PASSWORD_TYPE_MD5);
		std::cout << ":: Use MD5 passwords" << std::endl;
	}
	else if(asLowerCaseString(passwordType) == "sha1"){
		g_config.setNumber(ConfigManager::PASSWORD_TYPE, PASSWORD_TYPE_SHA1);
		std::cout << ":: Use SHA1 passwords" << std::endl;
	}
	else{
		ErrorMessage("Unknown password type!");
		exit(-1);
	}

	if(!g_game.loadMap(g_config.getString(ConfigManager::MAP_FILE),
		g_config.getString(ConfigManager::MAP_KIND))){
		exit(-1);
	}
	
	g_game.setGameState(GAME_STATE_INIT);
	g_game.loadPlayersRecord();

	std::pair<uint32_t, uint32_t> IpNetMask;
	IpNetMask.first  = inet_addr("127.0.0.1");
	IpNetMask.second = 0xFFFFFFFF;
	serverIPs.push_back(IpNetMask);
	
		char szHostName[128];
	if(gethostname(szHostName, 128) == 0){
		std::cout << "::" << std::endl << ":: Running on host " << szHostName << std::endl;

		hostent *he = gethostbyname(szHostName);

		if(he){
			std::cout << ":: Local IP address(es):  ";
			unsigned char** addr = (unsigned char**)he->h_addr_list;

			while (addr[0] != NULL){
				std::cout << (unsigned int)(addr[0][0]) << "."
				<< (unsigned int)(addr[0][1]) << "."
				<< (unsigned int)(addr[0][2]) << "."
				<< (unsigned int)(addr[0][3]) << "  ";

				IpNetMask.first  = *(uint32_t*)(*addr);
				IpNetMask.second = 0x0000FFFF;
				serverIPs.push_back(IpNetMask);

				addr++;
			}

			std::cout << std::endl;
		}
	}

	std::cout << ":: Local port:            ";
	int port = g_config.getNumber(ConfigManager::PORT);
	std::cout << port << std::endl;

	std::cout << ":: Global IP address:     ";
	std::string ip = g_config.getString(ConfigManager::IP);

	uint32_t resolvedIp = inet_addr(ip.c_str());
	if(resolvedIp == INADDR_NONE){
		struct hostent* he = gethostbyname(ip.c_str());
		if(he != 0){
			resolvedIp = *(uint32_t*)he->h_addr;
		}
		else{
			std::string error_msg = "Can't resolve: " + ip;
			ErrorMessage(error_msg.c_str());
			exit(-1);
		}
	}

		char resolvedIpstr[32];
	formatIP(resolvedIp, resolvedIpstr);
	std::cout << resolvedIpstr << std::endl << "::" << std::endl;

	IpNetMask.first  = resolvedIp;
	IpNetMask.second = 0;
	serverIPs.push_back(IpNetMask);
	std::cout << ":: Starting Server... ";

	Status* status = Status::instance();
	status->setMaxPlayersOnline(g_config.getNumber(ConfigManager::MAX_PLAYERS));

	g_game.setGameState(GAME_STATE_NORMAL);
	g_loaderSignal.notify_all();
}
