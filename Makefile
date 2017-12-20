CC=gcc
# XX=/usr/sup/llvm-5.0.0/bin/clang++.sh
# XX=clang++
XX=g++
IFLAGS= -I . 
CFLAGS= -g -Wall -Wextra $(IFLAGS)
CXXFLAGS= -std=c++14 -fira-algorithm=priority -g -Wall -Wextra $(IFLAGS)
LFLAGS= -lpthread

STORAGE= applications/storage/
COM_AP=  common/application/
COM_DS=  common/dstage/

all: $(STORAGE)storage_client \
  $(COM_AP)configreader.o $(COM_AP)test_configreader \
  $(COM_DS)job.o \
  $(COM_DS)duplicatestage.o \
  $(COM_DS)dispatcher.o $(COM_DS)test_dispatcher \
  $(COM_DS)multiqueue.o $(COM_DS)test_multiqueue

# *********************** TARGETS ********************
# common\applications
$(STORAGE)storage_client: $(STORAGE)storage_client.cpp $(COM_AP)configreader.o
	$(XX) -o $@ $< $(CXXFLAGS) $(COM_AP)configreader.o


# common\applications
$(COM_AP)configreader.o: $(COM_AP)configreader.cpp $(COM_AP)configreader.h
	$(XX) -c -o $@ $< $(CXXFLAGS)

$(COM_AP)test_configreader: $(COM_AP)test_configreader.cpp \
		$(COM_AP)configreader.o
	$(XX) -o $@ $< $(CXXFLAGS) $(LFLAGS) $(COM_AP)configreader.o


# common\dstagegit 
$(COM_DS)job.o: $(COM_DS)job.cpp $(COM_DS)job.h $(COM_DS)priority.h
	$(XX) -c -o $@ $< $(CXXFLAGS)

$(COM_DS)duplicatestage.o: $(COM_DS)duplicatestage.cpp \
		$(COM_DS)duplicatestage.h $(COM_DS)dstage.h $(COM_DS)job.o \
		$(COM_DS)multiqueue.o
	$(XX) -c -o $@ $< $(CXXFLAGS)

$(COM_DS)multiqueue.o: $(COM_DS)multiqueue.cpp \
		$(COM_DS)multiqueue.h $(COM_DS)job.o
	$(XX) -c -o $@ $< $(CXXFLAGS)

$(COM_DS)test_multiqueue: $(COM_DS)test_multiqueue.cpp
	$(XX) -o $@ $< $(CXXFLAGS) $(LFLAGS) $(COM_DS)job.o $(COM_DS)multiqueue.o

$(COM_DS)dispatcher.o: $(COM_DS)dispatcher.cpp \
		$(COM_DS)dispatcher.h $(COM_DS)dstage.h $(COM_DS)duplicatestage.o \
		$(COM_DS)multiqueue.o
	$(XX) -c -o $@ $< $(CXXFLAGS)

$(COM_DS)test_dispatcher: $(COM_DS)test_dispatcher.cpp
	$(XX) -o $@ $< $(CXXFLAGS) $(LFLAGS) $(COM_DS)dispatcher.o $(COM_DS)job.o $(COM_DS)multiqueue.o

format:
	clang-format -i  `find . -type f | command grep  '\.h\|\.cpp'`

clean:
	rm $(STORAGE)storage_client \
  $(COM_AP)configreader.o $(COM_AP)test_configreader \
  $(COM_DS)job.o $(COM_DS)duplicatestage.o \
  $(COM_DS)dispatcher.o $(COM_DS)test_dispatcher \
  $(COM_DS)multiqueue.o $(COM_DS)test_multiqueue


 