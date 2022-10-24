SRCDIR			= src
OUTDIR			= out
OBJDIR			= $(OUTDIR)/obj
SRCS			= $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/**/*.cpp) $(wildcard $(SRCDIR)/**/**/*.cpp)
OBJS			= $(subst $(SRCDIR), $(OBJDIR), $(SRCS:.cpp=.o))
TARGET			= $(OUTDIR)/main
CC				= g++
CFLAGS			= -std=c++17 -Wall -O2
CFLAGS_DEBUG	= -std=c++17 -Wall -O0 -g

main: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

debug: $(OBJS)
	$(CC) $(CFLAGS_DEBUG) -o $(TARGET) $^

TESTDIR 		= test
TEST_SRCDIR		= $(TESTDIR)/src
TEST_OUTDIR		= $(TESTDIR)/out
TEST_SRCS		= $(wildcard $(TEST_SRCDIR)/*.cpp)
TEST_TARGETS	= $(subst $(TEST_SRCDIR), $(TEST_OUTDIR), $(TEST_SRCS:.cpp=))
TEST_CFLAGS		= -std=c++17 -Wall -pthread -lgtest_main -lgtest

test: $(TEST_TARGETS)

$(TEST_OUTDIR)/uecda_state_test:	$(TEST_SRCDIR)/uecda_state_test.cpp $(OBJDIR)/uecda_cpp/hand.o $(OBJDIR)/uecda_cpp/cards.o $(OBJDIR)/uecda_state.o
	$(CC) $(TEST_CFLAGS) -o $@ $^

$(TEST_OUTDIR)/search_winning_hand_test:	$(TEST_SRCDIR)/search_winning_hand_test.cpp $(OBJDIR)/uecda_cpp/hand.o $(OBJDIR)/uecda_cpp/cards.o $(OBJDIR)/uecda_state.o $(OBJDIR)/search_winning_hand.o
	$(CC) $(TEST_CFLAGS) -o $@ $^

LEARNDIR 		= learn
LEARN_SRCDIR	= $(LEARNDIR)/src
LEARN_OUTDIR	= $(LEARNDIR)/out
LEARN_SRCS		= $(wildcard $(LEARN_SRCDIR)/*.cpp)
LEARN_TARGETS	= $(subst $(LEARN_SRCDIR), $(LEARN_OUTDIR), $(LEARN_SRCS:.cpp=))
LEARN_CFLAGS	= -std=c++17 -Wall -O2

learn: $(LEARN_TARGETS)

$(LEARN_OUTDIR)/learn_playout_policy:	$(LEARN_SRCDIR)/learn_playout_policy.cpp $(OBJDIR)/uecda_state.o $(OBJDIR)/uecda_cpp/hand.o $(OBJDIR)/uecda_cpp/cards.o
	$(CC) $(LEARN_CFLAGS) -o $@ $^

clean:
	rm -f ./out/main ./out/obj/**/*.o ./out/obj/*.o ./test/out/uecda_state_test
