#pragma once

#include "../containers/string.hpp"
#include "../containers/vector.hpp"

#include "intent_classifier.hpp"

template <typename LabelType> struct TrainingExample {
  String text;
  LabelType label;

  TrainingExample() = default;

  TrainingExample(String text, LabelType label) : text(text), label(label) {}
};

template <typename LabelType> class TrainingDataset {
private:
  Vector<TrainingExample<LabelType>> examples;

public:
  void add(String text, LabelType label) {
    examples.push_back(TrainingExample<LabelType>(text, label));
  }

  int size() const { return examples.size(); }

  TrainingExample<LabelType> &get(int index) { return examples[index]; }

  const TrainingExample<LabelType> &get(int index) const {
    return examples[index];
  }

  Vector<TrainingExample<LabelType>> &all() { return examples; }

  const Vector<TrainingExample<LabelType>> &all() const { return examples; }

  void register_examples() {
    // add(String text, LabelType label)
    add(String("hello"), IntentClassifier::Intent::GREETING);
    add(String("hi"), IntentClassifier::Intent::GREETING);
    add(String("hey"), IntentClassifier::Intent::GREETING);
    add(String("yo"), IntentClassifier::Intent::GREETING);
    add(String("hey rock"), IntentClassifier::Intent::GREETING);
    add(String("hello rock"), IntentClassifier::Intent::GREETING);
    add(String("good morning"), IntentClassifier::Intent::GREETING);
    add(String("good evening"), IntentClassifier::Intent::GREETING);
    add(String("what's up"), IntentClassifier::Intent::GREETING);
    add(String("how are you"), IntentClassifier::Intent::GREETING);
    add(String("rock on"), IntentClassifier::Intent::GREETING);
    add(String("hey Rock OS"), IntentClassifier::Intent::GREETING);
    add(String("yo Damian"), IntentClassifier::Intent::GREETING);
    add(String("wake up Rock"), IntentClassifier::Intent::GREETING);
    add(String("ready to shred?"), IntentClassifier::Intent::GREETING);
    add(String("greetings"), IntentClassifier::Intent::GREETING);
    add(String("hi there"), IntentClassifier::Intent::GREETING);
    add(String("hello there"), IntentClassifier::Intent::GREETING);
    add(String("good afternoon"), IntentClassifier::Intent::GREETING);
    add(String("howdy"), IntentClassifier::Intent::GREETING);
    add(String("sup"), IntentClassifier::Intent::GREETING);
    add(String("hey assistant"), IntentClassifier::Intent::GREETING);
    add(String("hi rock ai"), IntentClassifier::Intent::GREETING);
    add(String("good day"), IntentClassifier::Intent::GREETING);
    add(String("hey system"), IntentClassifier::Intent::GREETING);
    add(String("hi Garrick"), IntentClassifier::Intent::GREETING);
    add(String("hey Tyrant"), IntentClassifier::Intent::GREETING);
    add(String("hey Flamie"), IntentClassifier::Intent::GREETING);
    add(String("salutations"), IntentClassifier::Intent::GREETING);
    add(String("is anyone there"), IntentClassifier::Intent::GREETING);
    add(String("hello friend"), IntentClassifier::Intent::GREETING);
    add(String("hi buddy"), IntentClassifier::Intent::GREETING);
    add(String("hey team"), IntentClassifier::Intent::GREETING);
    add(String("top of the morning"),
                IntentClassifier::Intent::GREETING);
    add(String("how is it going"), IntentClassifier::Intent::GREETING);
    add(String("what is up rock"), IntentClassifier::Intent::GREETING);
    add(String("hey there buddy"), IntentClassifier::Intent::GREETING);
    add(String("well hello"), IntentClassifier::Intent::GREETING);
    add(String("hiya"), IntentClassifier::Intent::GREETING);
    add(String("greetings traveler"),
                IntentClassifier::Intent::GREETING);
    add(String("hello system"), IntentClassifier::Intent::GREETING);
    add(String("hi program"), IntentClassifier::Intent::GREETING);
    add(String("hey computer"), IntentClassifier::Intent::GREETING);
    add(String("yo machine"), IntentClassifier::Intent::GREETING);
    add(String("rise and shine"), IntentClassifier::Intent::GREETING);
    add(String("hello old friend"), IntentClassifier::Intent::GREETING);
    add(String("hi again"), IntentClassifier::Intent::GREETING);
    add(String("good to see you"), IntentClassifier::Intent::GREETING);
    add(String("nice to meet you"), IntentClassifier::Intent::GREETING);
    add(String("how do you do"), IntentClassifier::Intent::GREETING);
    add(String("reporting for duty"),
                IntentClassifier::Intent::GREETING);
    add(String("let's rock"), IntentClassifier::Intent::GREETING);
    add(String("ready to roll"), IntentClassifier::Intent::GREETING);
    add(String("hello digital world"),
                IntentClassifier::Intent::GREETING);
    add(String("hi folks"), IntentClassifier::Intent::GREETING);
    add(String("hey everyone"), IntentClassifier::Intent::GREETING);
    add(String("what is cracking"), IntentClassifier::Intent::GREETING);
    add(String("how are things"), IntentClassifier::Intent::GREETING);
    add(String("what's happening"), IntentClassifier::Intent::GREETING);
    add(String("long time no see"), IntentClassifier::Intent::GREETING);
    add(String("look who's here"), IntentClassifier::Intent::GREETING);
    add(String("hello gorgeous"), IntentClassifier::Intent::GREETING);
    add(String("hi boss"), IntentClassifier::Intent::GREETING);
    add(String("hey chief"), IntentClassifier::Intent::GREETING);
    add(String("morning rock"), IntentClassifier::Intent::GREETING);
    add(String("evening rock"), IntentClassifier::Intent::GREETING);
    add(String("night rock"), IntentClassifier::Intent::GREETING);
    add(String("yo rock os"), IntentClassifier::Intent::GREETING);
    add(String("hi rock os"), IntentClassifier::Intent::GREETING);
    add(String("hello digital assistant"),
                IntentClassifier::Intent::GREETING);
    add(String("hey AI"), IntentClassifier::Intent::GREETING);
    add(String("hi AI"), IntentClassifier::Intent::GREETING);
    add(String("greetings rock os"),
                IntentClassifier::Intent::GREETING);
    add(String("salutations rock"), IntentClassifier::Intent::GREETING);
    add(String("hey there rock"), IntentClassifier::Intent::GREETING);
    add(String("hi there rock os"), IntentClassifier::Intent::GREETING);
    add(String("hello there rock ai"),
                IntentClassifier::Intent::GREETING);
    add(String("good day rock"), IntentClassifier::Intent::GREETING);
    add(String("howdy partner"), IntentClassifier::Intent::GREETING);
    add(String("what's good"), IntentClassifier::Intent::GREETING);
    add(String("how goes it"), IntentClassifier::Intent::GREETING);
    add(String("is anyone home"), IntentClassifier::Intent::GREETING);
    add(String("hello anyone there"),
                IntentClassifier::Intent::GREETING);
    add(String("hi rock system"), IntentClassifier::Intent::GREETING);
    add(String("hey rock kernel"), IntentClassifier::Intent::GREETING);
    add(String("yo rock terminal"), IntentClassifier::Intent::GREETING);
    add(String("ready to jam"), IntentClassifier::Intent::GREETING);
    add(String("time to rock"), IntentClassifier::Intent::GREETING);
    add(String("let's get loud"), IntentClassifier::Intent::GREETING);
    add(String("hi Damian the tiger"),
                IntentClassifier::Intent::GREETING);
    add(String("hey Tyrant the crocodile"),
                IntentClassifier::Intent::GREETING);
    add(String("hello Garrick the lion"),
                IntentClassifier::Intent::GREETING);
    add(String("hi Flamie the dragon"),
                IntentClassifier::Intent::GREETING);
    add(String("what is shaking"), IntentClassifier::Intent::GREETING);
    add(String("how do you fare"), IntentClassifier::Intent::GREETING);
    add(String("well met"), IntentClassifier::Intent::GREETING);
    add(String("ahoy"), IntentClassifier::Intent::GREETING);
    add(String("show memory"), IntentClassifier::Intent::MEMORY_USAGE);
    add(String("show ram"), IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how much memory am i using"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how much RAM is free"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("check memory usage"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("is the system low on memory"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how much RAM is this beast using"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("check if Rock OS is eating all the memory"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("memory status"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("ram status"), IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how much memory is left"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("what is my current memory consumption"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("display ram usage"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("display memory stats"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how much ram are we burning through"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("is memory full"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("do we have enough ram"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("check ram"), IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how much memory do I have available"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("tell me memory usage"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("give me memory stats"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("are we running low on ram"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how many megabytes of ram are free"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how many gigabytes of memory are used"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("what's our ram usage right now"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("can you check the memory"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("please show memory usage"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("i need to see memory stats"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how heavy is the memory load"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("is the ram overloaded"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how much memory is consumed"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("report memory usage"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("report ram status"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("system memory info"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("system ram info"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how much ram is taken up"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how much memory is occupied"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("check available ram"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("check used memory"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how much headroom do we have in memory"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("is memory maxed out"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("is ram maxed out"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("what is the memory footprint"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("show me the ram allocation"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how much ram is allocated"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("give me a memory breakdown"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how much memory is free right now"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("track memory usage"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("monitor ram"), IntentClassifier::Intent::MEMORY_USAGE);
    add(String("is ram usage high"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("is memory usage high"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how much ram is being hogged"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("which apps are eating memory"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("what's eating up the ram"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how much ram is left in the tank"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("show system memory"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("show system ram"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("check total memory"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("check total ram"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how much memory does rock os take"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how much ram is rock os using"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("memory usage please"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("ram usage please"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("can i get memory stats"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("can i get ram stats"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("what's our memory status"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how's the ram looking"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how's memory looking"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("is ram running low"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("is memory running low"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how much ram is inactive"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how much memory is active"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("show active memory"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("show inactive memory"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("give me ram usage"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("give me memory usage"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("display ram stats"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("display memory usage"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how much ram is left over"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how much memory is left over"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("check system memory status"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("check system ram status"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("what is the current ram usage"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("what is the current memory usage"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how much memory is currently used"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("how much ram is currently used"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("is there any free memory"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("is there any free ram"),
                IntentClassifier::Intent::MEMORY_USAGE);
    add(String("what time is it"), IntentClassifier::Intent::TIME);
    add(String("what is the current time"),
                IntentClassifier::Intent::TIME);
    add(String("tell me the time"), IntentClassifier::Intent::TIME);
    add(String("check the clock"), IntentClassifier::Intent::TIME);
    add(String("show me the time"), IntentClassifier::Intent::TIME);
    add(String("what's the clock saying"),
                IntentClassifier::Intent::TIME);
    add(String("what time is the show?"),
                IntentClassifier::Intent::TIME);
    add(String("current time"), IntentClassifier::Intent::TIME);
    add(String("give me the time"), IntentClassifier::Intent::TIME);
    add(String("got the time"), IntentClassifier::Intent::TIME);
    add(String("do you know what time it is"),
                IntentClassifier::Intent::TIME);
    add(String("can you tell me the time"),
                IntentClassifier::Intent::TIME);
    add(String("what hour is it"), IntentClassifier::Intent::TIME);
    add(String("what is the time right now"),
                IntentClassifier::Intent::TIME);
    add(String("tell me what time it is"),
                IntentClassifier::Intent::TIME);
    add(String("clock check"), IntentClassifier::Intent::TIME);
    add(String("time please"), IntentClassifier::Intent::TIME);
    add(String("what's the time"), IntentClassifier::Intent::TIME);
    add(String("is it time yet"), IntentClassifier::Intent::TIME);
    add(String("what is the exact time"),
                IntentClassifier::Intent::TIME);
    add(String("give me the current time"),
                IntentClassifier::Intent::TIME);
    add(String("display the time"), IntentClassifier::Intent::TIME);
    add(String("display the clock"), IntentClassifier::Intent::TIME);
    add(String("show clock"), IntentClassifier::Intent::TIME);
    add(String("what time is it on the clock"),
                IntentClassifier::Intent::TIME);
    add(String("could you tell me the time"),
                IntentClassifier::Intent::TIME);
    add(String("would you tell me the time"),
                IntentClassifier::Intent::TIME);
    add(String("what is the hour right now"),
                IntentClassifier::Intent::TIME);
    add(String("tell me the current hour"),
                IntentClassifier::Intent::TIME);
    add(String("check time"), IntentClassifier::Intent::TIME);
    add(String("any idea what time it is"),
                IntentClassifier::Intent::TIME);
    add(String("do you have the time"), IntentClassifier::Intent::TIME);
    add(String("may I know the time"), IntentClassifier::Intent::TIME);
    add(String("what's the current hour and minute"),
                IntentClassifier::Intent::TIME);
    add(String("give me the clock time"),
                IntentClassifier::Intent::TIME);
    add(String("what's on the clock"), IntentClassifier::Intent::TIME);
    add(String("tell me what the clock says"),
                IntentClassifier::Intent::TIME);
    add(String("can I get the time"), IntentClassifier::Intent::TIME);
    add(String("need the time"), IntentClassifier::Intent::TIME);
    add(String("what time is it currently"),
                IntentClassifier::Intent::TIME);
    add(String("how late is it"), IntentClassifier::Intent::TIME);
    add(String("how early is it"), IntentClassifier::Intent::TIME);
    add(String("what time does the clock show"),
                IntentClassifier::Intent::TIME);
    add(String("read me the time"), IntentClassifier::Intent::TIME);
    add(String("report the time"), IntentClassifier::Intent::TIME);
    add(String("give me the hour"), IntentClassifier::Intent::TIME);
    add(String("what is the time of day"),
                IntentClassifier::Intent::TIME);
    add(String("can you check the time"),
                IntentClassifier::Intent::TIME);
    add(String("please tell me the time"),
                IntentClassifier::Intent::TIME);
    add(String("kindly tell me the time"),
                IntentClassifier::Intent::TIME);
    add(String("what's the time right now rock"),
                IntentClassifier::Intent::TIME);
    add(String("tell me the time rock os"),
                IntentClassifier::Intent::TIME);
    add(String("what time is it rock ai"),
                IntentClassifier::Intent::TIME);
    add(String("is it time for the concert"),
                IntentClassifier::Intent::TIME);
    add(String("what time does the gig start"),
                IntentClassifier::Intent::TIME);
    add(String("tell me the exact clock time"),
                IntentClassifier::Intent::TIME);
    add(String("display current time"), IntentClassifier::Intent::TIME);
    add(String("show system time"), IntentClassifier::Intent::TIME);
    add(String("what is the system time"),
                IntentClassifier::Intent::TIME);
    add(String("check system time"), IntentClassifier::Intent::TIME);
    add(String("give me system time"), IntentClassifier::Intent::TIME);
    add(String("what time is the system running on"),
                IntentClassifier::Intent::TIME);
    add(String("what is the local time"),
                IntentClassifier::Intent::TIME);
    add(String("check local time"), IntentClassifier::Intent::TIME);
    add(String("show local time"), IntentClassifier::Intent::TIME);
    add(String("what is the clock reading"),
                IntentClassifier::Intent::TIME);
    add(String("read the clock"), IntentClassifier::Intent::TIME);
    add(String("how many hours have passed today"),
                IntentClassifier::Intent::TIME);
    add(String("what is the timestamp"),
                IntentClassifier::Intent::TIME);
    add(String("give me the timestamp"),
                IntentClassifier::Intent::TIME);
    add(String("what is the time mark"),
                IntentClassifier::Intent::TIME);
    add(String("time check please"), IntentClassifier::Intent::TIME);
    add(String("quick time check"), IntentClassifier::Intent::TIME);
    add(String("can you give me the time"),
                IntentClassifier::Intent::TIME);
    add(String("would you check the time"),
                IntentClassifier::Intent::TIME);
    add(String("could you check the time"),
                IntentClassifier::Intent::TIME);
    add(String("tell me the hour and minute"),
                IntentClassifier::Intent::TIME);
    add(String("what's the precise time"),
                IntentClassifier::Intent::TIME);
    add(String("give me the precise time"),
                IntentClassifier::Intent::TIME);
    add(String("what is today's time"), IntentClassifier::Intent::TIME);
    add(String("show uptime"), IntentClassifier::Intent::UPTIME);
    add(String("what is the uptime"), IntentClassifier::Intent::UPTIME);
    add(String("how long has the system been running"),
                IntentClassifier::Intent::UPTIME);
    add(String("how long has Rock OS been up"),
                IntentClassifier::Intent::UPTIME);
    add(String("check system uptime"),
                IntentClassifier::Intent::UPTIME);
    add(String("when did the system start running"),
                IntentClassifier::Intent::UPTIME);
    add(String("how long has this machine been shredding"),
                IntentClassifier::Intent::UPTIME);
    add(String("how long has Rock OS been on stage"),
                IntentClassifier::Intent::UPTIME);
    add(String("system uptime"), IntentClassifier::Intent::UPTIME);
    add(String("what's our uptime"), IntentClassifier::Intent::UPTIME);
    add(String("how long has the computer been on"),
                IntentClassifier::Intent::UPTIME);
    add(String("how long has rock os been running"),
                IntentClassifier::Intent::UPTIME);
    add(String("tell me the uptime"), IntentClassifier::Intent::UPTIME);
    add(String("give me system uptime"),
                IntentClassifier::Intent::UPTIME);
    add(String("display uptime"), IntentClassifier::Intent::UPTIME);
    add(String("check uptime"), IntentClassifier::Intent::UPTIME);
    add(String("how long has this system been awake"),
                IntentClassifier::Intent::UPTIME);
    add(String("how long has rock os been active"),
                IntentClassifier::Intent::UPTIME);
    add(String("when was the system booted"),
                IntentClassifier::Intent::UPTIME);
    add(String("what is the boot time"),
                IntentClassifier::Intent::UPTIME);
    add(String("how long since last reboot"),
                IntentClassifier::Intent::UPTIME);
    add(String("how long since the last boot"),
                IntentClassifier::Intent::UPTIME);
    add(String("uptime stats"), IntentClassifier::Intent::UPTIME);
    add(String("show system uptime stats"),
                IntentClassifier::Intent::UPTIME);
    add(String("how many hours has rock os been running"),
                IntentClassifier::Intent::UPTIME);
    add(String("how many days has the system been up"),
                IntentClassifier::Intent::UPTIME);
    add(String("what's the machine's uptime"),
                IntentClassifier::Intent::UPTIME);
    add(String("how long has this beast been running"),
                IntentClassifier::Intent::UPTIME);
    add(String("how long has the amp been powered on"),
                IntentClassifier::Intent::UPTIME);
    add(String("give me the uptime stats"),
                IntentClassifier::Intent::UPTIME);
    add(String("can you check uptime"),
                IntentClassifier::Intent::UPTIME);
    add(String("please show uptime"), IntentClassifier::Intent::UPTIME);
    add(String("i want to know the uptime"),
                IntentClassifier::Intent::UPTIME);
    add(String("tell me how long rock os has been running"),
                IntentClassifier::Intent::UPTIME);
    add(String("how long has the OS been online"),
                IntentClassifier::Intent::UPTIME);
    add(String("how long has the system been online"),
                IntentClassifier::Intent::UPTIME);
    add(String("check how long we've been running"),
                IntentClassifier::Intent::UPTIME);
    add(String("check how long rock os has been up"),
                IntentClassifier::Intent::UPTIME);
    add(String("what is the current system uptime"),
                IntentClassifier::Intent::UPTIME);
    add(String("report system uptime"),
                IntentClassifier::Intent::UPTIME);
    add(String("report uptime"), IntentClassifier::Intent::UPTIME);
    add(String("how long has this session lasted"),
                IntentClassifier::Intent::UPTIME);
    add(String("session duration"), IntentClassifier::Intent::UPTIME);
    add(String("how long have we been rocking"),
                IntentClassifier::Intent::UPTIME);
    add(String("how long has the concert been going"),
                IntentClassifier::Intent::UPTIME);
    add(String("how long has the show been running"),
                IntentClassifier::Intent::UPTIME);
    add(String("what's the duration of this boot"),
                IntentClassifier::Intent::UPTIME);
    add(String("how long has the kernel been running"),
                IntentClassifier::Intent::UPTIME);
    add(String("check kernel uptime"),
                IntentClassifier::Intent::UPTIME);
    add(String("show boot duration"), IntentClassifier::Intent::UPTIME);
    add(String("how long since bootup"),
                IntentClassifier::Intent::UPTIME);
    add(String("time since boot"), IntentClassifier::Intent::UPTIME);
    add(String("give me time since boot"),
                IntentClassifier::Intent::UPTIME);
    add(String("how long has the PC been running"),
                IntentClassifier::Intent::UPTIME);
    add(String("how long has the workstation been up"),
                IntentClassifier::Intent::UPTIME);
    add(String("can you tell me the uptime"),
                IntentClassifier::Intent::UPTIME);
    add(String("would you check the uptime"),
                IntentClassifier::Intent::UPTIME);
    add(String("could you show me the system uptime"),
                IntentClassifier::Intent::UPTIME);
    add(String("how long has rock os been powered on"),
                IntentClassifier::Intent::UPTIME);
    add(String("what's the total uptime"),
                IntentClassifier::Intent::UPTIME);
    add(String("total system uptime"),
                IntentClassifier::Intent::UPTIME);
    add(String("how many minutes has rock os been running"),
                IntentClassifier::Intent::UPTIME);
    add(String("how many seconds has the system been up"),
                IntentClassifier::Intent::UPTIME);
    add(String("track system uptime"),
                IntentClassifier::Intent::UPTIME);
    add(String("monitor uptime"), IntentClassifier::Intent::UPTIME);
    add(String("is there an uptime counter"),
                IntentClassifier::Intent::UPTIME);
    add(String("show me the uptime counter"),
                IntentClassifier::Intent::UPTIME);
    add(String("how long has this os been live"),
                IntentClassifier::Intent::UPTIME);
    add(String("how long has rock os been live"),
                IntentClassifier::Intent::UPTIME);
    add(String("what is the running time"),
                IntentClassifier::Intent::UPTIME);
    add(String("give me the running time"),
                IntentClassifier::Intent::UPTIME);
    add(String("how long has the system been operational"),
                IntentClassifier::Intent::UPTIME);
    add(String("operational time"), IntentClassifier::Intent::UPTIME);
    add(String("show operational time"),
                IntentClassifier::Intent::UPTIME);
    add(String("how long has rock os been operational"),
                IntentClassifier::Intent::UPTIME);
    add(String("check operational time"),
                IntentClassifier::Intent::UPTIME);
    add(String("open terminal"), IntentClassifier::Intent::OPEN_APP);
    add(String("launch terminal"), IntentClassifier::Intent::OPEN_APP);
    add(String("start terminal"), IntentClassifier::Intent::OPEN_APP);
    add(String("bring up terminal"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("can you open the terminal"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("i want to use the terminal"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("launch the browser"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("start the clock"), IntentClassifier::Intent::OPEN_APP);
    add(String("open settings"), IntentClassifier::Intent::OPEN_APP);
    add(String("fire up the amp"), IntentClassifier::Intent::OPEN_APP);
    add(String("launch the tuner"), IntentClassifier::Intent::OPEN_APP);
    add(String("bring up the playlist"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("start the Rock OS radio"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("open files"), IntentClassifier::Intent::OPEN_APP);
    add(String("launch files"), IntentClassifier::Intent::OPEN_APP);
    add(String("open paint"), IntentClassifier::Intent::OPEN_APP);
    add(String("open matrix"), IntentClassifier::Intent::OPEN_APP);
    add(String("launch tyrant"), IntentClassifier::Intent::OPEN_APP);
    add(String("open calculator"), IntentClassifier::Intent::OPEN_APP);
    add(String("open metronome"), IntentClassifier::Intent::OPEN_APP);
    add(String("open lyrics"), IntentClassifier::Intent::OPEN_APP);
    add(String("open mixer"), IntentClassifier::Intent::OPEN_APP);
    add(String("open store"), IntentClassifier::Intent::OPEN_APP);
    add(String("open vinyl"), IntentClassifier::Intent::OPEN_APP);
    add(String("open microphone"), IntentClassifier::Intent::OPEN_APP);
    add(String("open drum"), IntentClassifier::Intent::OPEN_APP);
    add(String("open recorder"), IntentClassifier::Intent::OPEN_APP);
    add(String("open equalizer"), IntentClassifier::Intent::OPEN_APP);
    add(String("open help"), IntentClassifier::Intent::OPEN_APP);
    add(String("start browser"), IntentClassifier::Intent::OPEN_APP);
    add(String("launch browser"), IntentClassifier::Intent::OPEN_APP);
    add(String("start paint"), IntentClassifier::Intent::OPEN_APP);
    add(String("launch paint"), IntentClassifier::Intent::OPEN_APP);
    add(String("start calculator"), IntentClassifier::Intent::OPEN_APP);
    add(String("launch calculator"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("start settings"), IntentClassifier::Intent::OPEN_APP);
    add(String("launch settings"), IntentClassifier::Intent::OPEN_APP);
    add(String("bring up settings"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("bring up browser"), IntentClassifier::Intent::OPEN_APP);
    add(String("bring up files"), IntentClassifier::Intent::OPEN_APP);
    add(String("start files"), IntentClassifier::Intent::OPEN_APP);
    add(String("open the browser app"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("open the terminal app"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("start the equalizer"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("launch the recorder"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("fire up the mixer"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("open up the drum machine"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("start up the vinyl player"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("open the store"), IntentClassifier::Intent::OPEN_APP);
    add(String("launch the lyrics app"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("open the metronome app"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("start the tuner app"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("bring up the clock app"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("open matrix screen"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("start tyrant app"), IntentClassifier::Intent::OPEN_APP);
    add(String("can you open the browser"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("can you launch the calculator"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("i need to open the terminal"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("i want to open files"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("please open settings"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("kindly launch terminal"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("run terminal"), IntentClassifier::Intent::OPEN_APP);
    add(String("run browser"), IntentClassifier::Intent::OPEN_APP);
    add(String("run calculator"), IntentClassifier::Intent::OPEN_APP);
    add(String("run paint"), IntentClassifier::Intent::OPEN_APP);
    add(String("execute terminal"), IntentClassifier::Intent::OPEN_APP);
    add(String("fire up terminal"), IntentClassifier::Intent::OPEN_APP);
    add(String("fire up browser"), IntentClassifier::Intent::OPEN_APP);
    add(String("fire up files"), IntentClassifier::Intent::OPEN_APP);
    add(String("spin up the browser"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("spin up terminal"), IntentClassifier::Intent::OPEN_APP);
    add(String("load terminal"), IntentClassifier::Intent::OPEN_APP);
    add(String("load browser"), IntentClassifier::Intent::OPEN_APP);
    add(String("load files"), IntentClassifier::Intent::OPEN_APP);
    add(String("open music playlist"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("open radio app"), IntentClassifier::Intent::OPEN_APP);
    add(String("open microphone app"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("open recorder app"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("open equalizer app"),
                IntentClassifier::Intent::OPEN_APP);
    add(String("close window"), IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("close this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("exit this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("quit this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("shut this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("get rid of this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("close the current window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("take this window off the stage"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("get this window out of here"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("kill this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("send this window backstage"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("close"), IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("exit"), IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("quit"), IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("shut window"), IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("shut down this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("kill the active window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("close active window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("terminate this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("terminate the current window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("destroy this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("dismiss this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("dismiss current window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("stop this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("end this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("shut it down"), IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("close this app window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("exit this application"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("quit this application"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("close application"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("can you close this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("can you exit this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("please close this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("please kill this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("i want to close this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("i need to close this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("shut this app down"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("close window now"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("exit window now"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("quit window now"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("get rid of this app window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("take this window down"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("drop this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("drop the current window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("close this program"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("exit this program"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("quit this program"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("kill this program window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("force close this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("force exit this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("force quit this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("close out of this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("exit out of this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("shut the window down"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("power down this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("cut this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("cut the current window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("wrap up this window and close it"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("close window please"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("exit window please"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("quit window please"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("close this panel"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("close this screen"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("exit this screen"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("shut this screen down"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("kill this screen"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("get this screen out of here"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("send this window to the graveyard"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("destroy the active window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("close this up"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("shut this up"), IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("end the current window session"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("stop current window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("halt this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("abort this window"),
                IntentClassifier::Intent::CLOSE_WINDOW);
    add(String("minimize window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("minimize this window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("hide this window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("put this window away"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("send this window to the taskbar"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("take this window backstage"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("get this window off the stage for now"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("hide this window but don't close it"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("minimize"), IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("hide"), IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("minimize current window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("hide current window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("send window to taskbar"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("send current window to taskbar"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("put window away"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("tuck this window away"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("tuck away this window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("collapse this window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("collapse window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("iconify this window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("iconify window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("send this window to the dock"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("send this window down to the taskbar"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("can you minimize this window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("can you hide this window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("please minimize this window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("please hide this window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("i want to minimize this window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("i need to hide this window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("minimize this app window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("hide this app window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("minimize window now"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("hide window now"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("drop this window to the taskbar"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("send this window to the background"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("put this window in the background"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("minimize the active window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("hide the active window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("shove this window into the taskbar"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("stash this window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("stash away this window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("park this window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("park this window in the taskbar"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("keep this window but hide it"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("keep window hidden"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("minimize please"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("hide please"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("minimize this screen"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("hide this screen"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("send this screen to the taskbar"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("put this screen away"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("make this window disappear"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("make this window go away temporarily"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("send this window out of sight"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("minimize window please"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("hide window please"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("could you minimize this window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("would you hide this window"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("minimize the window for a second"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("hide the window for now"),
                IntentClassifier::Intent::MINIMIZE_WINDOW);
    add(String("help"), IntentClassifier::Intent::HELP);
    add(String("help me"), IntentClassifier::Intent::HELP);
    add(String("i need help"), IntentClassifier::Intent::HELP);
    add(String("what can you do"), IntentClassifier::Intent::HELP);
    add(String("show help"), IntentClassifier::Intent::HELP);
    add(String("show commands"), IntentClassifier::Intent::HELP);
    add(String("what commands are available"),
                IntentClassifier::Intent::HELP);
    add(String("how do i use this"), IntentClassifier::Intent::HELP);
    add(String("what features do you have"),
                IntentClassifier::Intent::HELP);
    add(String("show me how to rock this system"),
                IntentClassifier::Intent::HELP);
    add(String("what can Rock AI do"), IntentClassifier::Intent::HELP);
    add(String("give me the Rock OS command list"),
                IntentClassifier::Intent::HELP);
    add(String("how do I use this thing"),
                IntentClassifier::Intent::HELP);
    add(String("assistance"), IntentClassifier::Intent::HELP);
    add(String("i need assistance"), IntentClassifier::Intent::HELP);
    add(String("can you assist me"), IntentClassifier::Intent::HELP);
    add(String("give me help"), IntentClassifier::Intent::HELP);
    add(String("guide me"), IntentClassifier::Intent::HELP);
    add(String("show guide"), IntentClassifier::Intent::HELP);
    add(String("what are my options"), IntentClassifier::Intent::HELP);
    add(String("what can i say to you"),
                IntentClassifier::Intent::HELP);
    add(String("what kind of commands do you accept"),
                IntentClassifier::Intent::HELP);
    add(String("list all commands"), IntentClassifier::Intent::HELP);
    add(String("show command list"), IntentClassifier::Intent::HELP);
    add(String("display help menu"), IntentClassifier::Intent::HELP);
    add(String("open help menu"), IntentClassifier::Intent::HELP);
    add(String("how does rock os work"),
                IntentClassifier::Intent::HELP);
    add(String("explain how to use rock os"),
                IntentClassifier::Intent::HELP);
    add(String("what instructions are available"),
                IntentClassifier::Intent::HELP);
    add(String("give me instructions"), IntentClassifier::Intent::HELP);
    add(String("i am lost, help"), IntentClassifier::Intent::HELP);
    add(String("i need a manual"), IntentClassifier::Intent::HELP);
    add(String("where is the manual"), IntentClassifier::Intent::HELP);
    add(String("show manual"), IntentClassifier::Intent::HELP);
    add(String("can you give me a hand"),
                IntentClassifier::Intent::HELP);
    add(String("i could use some help"),
                IntentClassifier::Intent::HELP);
    add(String("what are your capabilities"),
                IntentClassifier::Intent::HELP);
    add(String("tell me what you can do"),
                IntentClassifier::Intent::HELP);
    add(String("list your features"), IntentClassifier::Intent::HELP);
    add(String("show system capabilities"),
                IntentClassifier::Intent::HELP);
    add(String("how do i operate rock os"),
                IntentClassifier::Intent::HELP);
    add(String("what is rock ai capable of"),
                IntentClassifier::Intent::HELP);
    add(String("give me a rundown of commands"),
                IntentClassifier::Intent::HELP);
    add(String("show me what you can do"),
                IntentClassifier::Intent::HELP);
    add(String("can you show me available commands"),
                IntentClassifier::Intent::HELP);
    add(String("please help me"), IntentClassifier::Intent::HELP);
    add(String("kindly provide assistance"),
                IntentClassifier::Intent::HELP);
    add(String("i need some guidance"), IntentClassifier::Intent::HELP);
    add(String("what is this operating system capable of"),
                IntentClassifier::Intent::HELP);
    add(String("how do i interact with rock os"),
                IntentClassifier::Intent::HELP);
    add(String("is there a help guide"),
                IntentClassifier::Intent::HELP);
    add(String("show support"), IntentClassifier::Intent::HELP);
    add(String("support menu"), IntentClassifier::Intent::HELP);
    add(String("get help"), IntentClassifier::Intent::HELP);
    add(String("need assistance please"),
                IntentClassifier::Intent::HELP);
    add(String("what commands can i type"),
                IntentClassifier::Intent::HELP);
    add(String("show me the command reference"),
                IntentClassifier::Intent::HELP);
    add(String("command reference"), IntentClassifier::Intent::HELP);
    add(String("how do i run apps here"),
                IntentClassifier::Intent::HELP);
    add(String("what can you do for me"),
                IntentClassifier::Intent::HELP);
    add(String("show me the ropes"), IntentClassifier::Intent::HELP);
    add(String("teach me how to rock"), IntentClassifier::Intent::HELP);
    add(String("give me a tutorial"), IntentClassifier::Intent::HELP);
    add(String("start tutorial"), IntentClassifier::Intent::HELP);
    add(String("how do i get started"), IntentClassifier::Intent::HELP);
    add(String("getting started guide"),
                IntentClassifier::Intent::HELP);
    add(String("show getting started"), IntentClassifier::Intent::HELP);
    add(String("what is the help protocol"),
                IntentClassifier::Intent::HELP);
    add(String("display system help"), IntentClassifier::Intent::HELP);
    add(String("display available commands"),
                IntentClassifier::Intent::HELP);
    add(String("list available actions"),
                IntentClassifier::Intent::HELP);
    add(String("what actions can you perform"),
                IntentClassifier::Intent::HELP);
    add(String("explain system features"),
                IntentClassifier::Intent::HELP);
    add(String("help options"), IntentClassifier::Intent::HELP);
    add(String("what assistance is available"),
                IntentClassifier::Intent::HELP);
    add(String("i need help figuring this out"),
                IntentClassifier::Intent::HELP);
    add(String("can you explain how this works"),
                IntentClassifier::Intent::HELP);
    add(String("tell me how to use rock os properly"),
                IntentClassifier::Intent::HELP);
    add(String("show me assistance options"),
                IntentClassifier::Intent::HELP);
    add(String("give me help with commands"),
                IntentClassifier::Intent::HELP);
    add(String("what is the purpose of this ai"),
                IntentClassifier::Intent::HELP);
    add(String("how can rock ai help me"),
                IntentClassifier::Intent::HELP);
    add(String("show me the help screen"),
                IntentClassifier::Intent::HELP);
    add(String("open help screen"), IntentClassifier::Intent::HELP);
    add(String("pull up help"), IntentClassifier::Intent::HELP);
    add(String("pull up the command list"),
                IntentClassifier::Intent::HELP);
  }
};
