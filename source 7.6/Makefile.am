
noinst_PROGRAMS = otserv

AM_CXXFLAGS = $(XML_CPPFLAGS) $(OTSERV_FLAGS) $(LUA_CFLAGS) $(DEBUG_FLAGS)\
	$(MYSQL_FLAGS) $(SQLITE_FLAGS) $(ODBC_FLAGS) $(PGSQL_FLAGS)\
	$(PROFILER_FLAGS) $(OPTIONAL_FLAGS) -D_THREAD_SAFE -D_REENTRANT -Wall

otserv_LDADD = $(LUA_LIBS)

if USE_MYSQL
MAYBE_MYSQL = databasemysql.cpp databasemysql.h
endif
if USE_SQLITE
MAYBE_SQLITE = databasesqlite.cpp databasesqlite.h
endif
if USE_ODBC
MAYBE_ODBC = databaseodbc.cpp databaseodbc.h
endif
if USE_PGSQL
MAYBE_PGSQL = databasepgsql.cpp databasepgsql.h
endif

if NEED_ENCRYPTION
MAYBE_ENCRYPTION = rsa.cpp rsa.h
endif

otserv_SOURCES = account.cpp actions.cpp allocator.cpp ban.cpp baseevents.cpp beds.cpp \
	chat.cpp combat.cpp commands.cpp condition.cpp configmanager.cpp connection.cpp \
	container.cpp creature.cpp creatureevent.cpp cylinder.cpp database.cpp \
	databasemysql.cpp databasesqlite.cpp depot.cpp exception.cpp fileloader.cpp \
	game.cpp house.cpp housetile.cpp ioaccount.cpp iomapotbm.cpp iomapserialize.cpp \
        iomapxml.cpp ioplayer.cpp item.cpp items.cpp logger.cpp luascript.cpp mailbox.cpp \
	map.cpp md5.cpp monster.cpp monsters.cpp movement.cpp networkmessage.cpp npc.cpp \
	otpch.cpp otserv.cpp outfit.cpp outputmessage.cpp party.cpp player.cpp position.cpp \
	protocol76.cpp protocol.cpp protocollogin.cpp raids.cpp scheduler.cpp scriptmanager.cpp \
	server.cpp sha1.cpp spawn.cpp spells.cpp status.cpp talkaction.cpp tasks.cpp \
	teleport.cpp thing.cpp tile.cpp tools.cpp trashholder.cpp vocation.cpp waitlist.cpp weapons.cpp \
	account.h actions.h allocator.h ban.h baseevents.h beds.h chat.h combat.h commands.h condition.h \
	config.h config.h.in config.h.in~ configmanager.h connection.h const76.h container.h creatureevent.h \
	creature.h cylinder.h database.h databasemysql.h databasesqlite.h definitions.h depot.h enums.h \
	exception.h fileloader.h game.h house.h housetile.h ioaccount.h iomap.h iomapotbm.h iomapserialize.h \
	iomapxml.h ioplayer.h item.h itemloader.h items.h logger.h luascript.h mailbox.h map.h md5.h \
	monster.h monsters.h movement.h networkmessage.h npc.h otpch.h otsystem.h outfit.h outputmessage.h \
	party.h player.h position.h protocol76.h protocol.h protocollogin.h raids.h scheduler.h \
	scriptmanager.h server.h sha1.h spawn.h spells.h status.h talkaction.h tasks.h teleport.h \
	templates.h thing.h tile.h tools.h town.h trashholder.h vocation.h waitlist.h waypoints.h weapons.h

	
