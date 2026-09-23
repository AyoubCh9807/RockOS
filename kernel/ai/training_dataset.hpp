#pragma once

#include "../containers/string.hpp"
#include "../containers/vector.hpp"

#include "intent_classifier.hpp"
#include "tokenizer.hpp"
#include "vocabulary.hpp"

template <typename IntentType, typename EntityType> struct TrainingExample {
  String text;
  IntentType intent;
  EntityType entity;

  TrainingExample() = default;

  TrainingExample(String text, IntentType intent, EntityType entity)
      : text(text), intent(intent), entity(entity) {}
};

class TrainingDataset {
private:
  using Example =
      TrainingExample<IntentClassifier::Intent, IntentClassifier::Entity>;

  Vector<Example> examples;

  Vocabulary &vocab;
  Tokenizer &tokenizer;

public:
  TrainingDataset(Tokenizer &tokenizer, Vocabulary &vocab)
      : vocab(vocab), tokenizer(tokenizer) {}

  void add(String text, IntentClassifier::Intent intent,
           IntentClassifier::Entity entity) {

    examples.push_back(Example(text, intent, entity));

    Vector<Token> &tokens = tokenizer.tokenize(text);

    for (auto &t : tokens)
      vocab.add(t.content);
  }

  int size() const { return examples.size(); }

  Example &get(int index) { return examples[index]; }

  const Example &get(int index) const { return examples[index]; }

  void register_examples() {
    add("hello", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hi there", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("yo", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("what's up", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("good morning", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey rock ai", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hi rock ai", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("greetings", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("whats up", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey there assistant", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hello rock os", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("sup", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("good afternoon", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("good evening", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hiya", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hello there", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("heyyo", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("salutations", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("yo rock ai", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hello friend", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("morning", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("evening", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hias", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey assistant", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("howdy", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hi rock", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey system", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("oh hey", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey whats up", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hello companion", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey there buddy", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hi man", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("yo what is good", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey rock os", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hello again", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hi ai", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey there pal", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("ahoy", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hello listener", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("what is up rock ai", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey you", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hi there rock os", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("yo rock", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hello program", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey rockai", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hi there ai", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("good day", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("heyho", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hello chief", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey rockai how are you", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hi back", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("yo assistant", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hello terminal friend", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey there system", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hi rock os user interface", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("what is up assistant", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hello bot", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey rock ai are you there", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hi buddy", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("yo what is up", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hello rock os core", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey mate", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hi there friend", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("oh hello", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey rock ai whats up", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hi again", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("yo what up", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hello rock os desktop", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey there user", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hi rock os", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("active greeting rock ai", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey rock ai greetings", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hi team", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("yo rock ai are you active", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hello digital friend", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey rock os assistant", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hi there rock ai", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("yo rock os", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hello human", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey rock ai hello", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hi rockai", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("yo what is happening", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hello rock os shell", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey there cool assistant", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hi rock os ai", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("yo rock ai what is up", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hello rock os system", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey rock ai hi", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hi there system ai", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("yo my friend", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hello rock os companion", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey rock os friend", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hi rock ai assistant", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("yo rock ai hello", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hello rock os interface", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey rock ai yo", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hi there rock os user", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("yo rock os assistant", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hello rock os kernel", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hey rock ai sup", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("hi rock os companion", IntentClassifier::Intent::GREETING,
        IntentClassifier::Entity::NONE);
    add("how much memory is being used", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("check RAM", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("memory usage", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much RAM is free", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("show memory consumption", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much memory Rock OS is using",
        IntentClassifier::Intent::MEMORY_USAGE, IntentClassifier::Entity::NONE);
    add("whats my ram usage", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("check memory", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much memory do we have left",
        IntentClassifier::Intent::MEMORY_USAGE, IntentClassifier::Entity::NONE);
    add("ram status", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("show ram usage", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how is memory looking", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("memory stats", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("what is my current memory consumption",
        IntentClassifier::Intent::MEMORY_USAGE, IntentClassifier::Entity::NONE);
    add("can you check the ram", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much memory is free", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("ram consumption", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("system memory usage", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("display ram usage", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much memory is taken up", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("tell me memory usage", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("is memory running low", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much ram is rock os using", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("check system memory", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("memory report", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much ram do i have", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("ram usage please", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("what amount of ram is used", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("show me the ram", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("memory usage stats", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much ram is occupied", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("check my memory please", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("ram usage report", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much memory is allocated", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("memory utilization", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("what is the ram utilization", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("show memory stats", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much ram is in use", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("memory check", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("can you see my ram usage", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much memory does rock os use",
        IntentClassifier::Intent::MEMORY_USAGE, IntentClassifier::Entity::NONE);
    add("display memory usage", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("ram check", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much memory is active", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("memory gauge", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("what is memory status", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("check the system ram", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much ram is available", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("memory monitor", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("show current memory usage", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much ram is currently used",
        IntentClassifier::Intent::MEMORY_USAGE, IntentClassifier::Entity::NONE);
    add("memory breakdown", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("tell me the ram usage", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("is ram full", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much memory is consumed", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("memory info", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("what is memory usage right now",
        IntentClassifier::Intent::MEMORY_USAGE, IntentClassifier::Entity::NONE);
    add("check ram usage please", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much memory does the system use",
        IntentClassifier::Intent::MEMORY_USAGE, IntentClassifier::Entity::NONE);
    add("ram statistics", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("show me memory usage", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much ram is taken", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("memory capacity check", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("what is our ram usage", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("check available memory", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much memory is left", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("ram status report", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("show system memory", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much memory is utilized", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("memory usage status", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("what is the memory consumption",
        IntentClassifier::Intent::MEMORY_USAGE, IntentClassifier::Entity::NONE);
    add("check memory usage right now", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much ram is free right now",
        IntentClassifier::Intent::MEMORY_USAGE, IntentClassifier::Entity::NONE);
    add("memory overview", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("show ram status", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much memory is working", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("memory use", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("what is the current ram status",
        IntentClassifier::Intent::MEMORY_USAGE, IntentClassifier::Entity::NONE);
    add("check ram consumption", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much memory is busy", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("memory metrics", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("show how much ram is used", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much memory is assigned", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("memory analysis", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("what is the ram level", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("check rock os memory", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much memory is holding data",
        IntentClassifier::Intent::MEMORY_USAGE, IntentClassifier::Entity::NONE);
    add("memory log", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("show me ram usage stats", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much memory is running", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("memory tracker", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("what is my memory level", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("check total memory usage", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("how much memory is in service", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("memory status check", IntentClassifier::Intent::MEMORY_USAGE,
        IntentClassifier::Entity::NONE);
    add("what is time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what time is it", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("tell me the time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("current time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("check the clock", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what hour is it", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("do you know the time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("whats the time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what time is it rn", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("yo whats the clock saying", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("give me the time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("can you tell me the time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what is the current time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("clock time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("show me the time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what time is it right now", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("tell me what time it is", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("time please", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what is the clock reading", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("could you tell me the time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what hour of the day is it", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("check time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("is it time yet", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what is the exact time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("time check", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("can you check the time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what time do you have", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("show time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("got the time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what is today's time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("display the time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("tell me what hour it is", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what's the clock say", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("time of day", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("can I get the time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what time is it on the clock", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("tell me current time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what is the time right now", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("clock please", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what is our current time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("check the system time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what time is on the system clock", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("how late is it", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what is the hour", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("can you give me the time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what time is registered", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("tell me what time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what time is showing", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("check clock", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what is the local time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("give me current time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what is the time indicator", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("show me current time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what time is currently set", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("tell me the clock", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what is the time today", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("time report", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what is the current clock time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("check system clock", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what time is it in rock os", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("tell me the exact time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what is the time status", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("can you tell me what time it is", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what time is recorded", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("display current time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what is the clock time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("tell me what time it is right now", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what is the time value", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("check the time please", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what time is displaying", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("give me the current time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what is the time reading", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("show time please", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what time does the clock say", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("tell me the time now", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what is the time update", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("check clock time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what time is on display", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("give me clock time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what is the time info", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("show system time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what time is active", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("tell me the system time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what is the time output", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("check current time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what time is configured", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("give me the system time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what is the time metric", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("show clock", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what time is running", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("tell me clock time", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what is the time format", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("check time please", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what time is operational", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("give me the clock", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("what is the time status report", IntentClassifier::Intent::TIME,
        IntentClassifier::Entity::NONE);
    add("how long has rock os been running", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("whats the uptime", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("when did the system boot", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long ago did I start Rock OS", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("show system uptime", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("system uptime", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has the system been on", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("check uptime", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long have we been running", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("uptime please", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has rock os been online", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("what is the system uptime", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long since boot", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("check system uptime", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has this session been active",
        IntentClassifier::Intent::UPTIME, IntentClassifier::Entity::NONE);
    add("show uptime", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has rock os been up", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("tell me the uptime", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has the computer been running",
        IntentClassifier::Intent::UPTIME, IntentClassifier::Entity::NONE);
    add("system run time", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has rock os been active", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("what time did we boot up", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has the OS been running", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("uptime stats", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("can you check the uptime", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has the kernel been running",
        IntentClassifier::Intent::UPTIME, IntentClassifier::Entity::NONE);
    add("display uptime", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long since last reboot", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("system run duration", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has rock os been powered on",
        IntentClassifier::Intent::UPTIME, IntentClassifier::Entity::NONE);
    add("uptime report", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has the system been active", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("check OS uptime", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how many hours has rock os been running",
        IntentClassifier::Intent::UPTIME, IntentClassifier::Entity::NONE);
    add("tell me system uptime", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has rock os been working", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("uptime information", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has the system been online", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("check how long rock os is running", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has session been running", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("uptime status", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has rock os been awake", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("show me the uptime", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has the system been up", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("system boot time", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has rock os been started", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("uptime check", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has the OS been online", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("what is our uptime", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long since system start", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("uptime metrics", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has rock os been operating", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("check the system uptime", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has the machine been running",
        IntentClassifier::Intent::UPTIME, IntentClassifier::Entity::NONE);
    add("uptime log", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has rock os been running for",
        IntentClassifier::Intent::UPTIME, IntentClassifier::Entity::NONE);
    add("tell me how long rock os is on", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has system execution lasted",
        IntentClassifier::Intent::UPTIME, IntentClassifier::Entity::NONE);
    add("uptime summary", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has rock os been operational",
        IntentClassifier::Intent::UPTIME, IntentClassifier::Entity::NONE);
    add("check current uptime", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has the platform been running",
        IntentClassifier::Intent::UPTIME, IntentClassifier::Entity::NONE);
    add("uptime details", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has rock os been live", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("show me system uptime", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long since startup", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("uptime analysis", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has rock os been functioning",
        IntentClassifier::Intent::UPTIME, IntentClassifier::Entity::NONE);
    add("check uptime stats", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has the device been running",
        IntentClassifier::Intent::UPTIME, IntentClassifier::Entity::NONE);
    add("uptime record", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has rock os been awake and running",
        IntentClassifier::Intent::UPTIME, IntentClassifier::Entity::NONE);
    add("tell me the system uptime stats", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has the environment been active",
        IntentClassifier::Intent::UPTIME, IntentClassifier::Entity::NONE);
    add("uptime status check", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has rock os been running today",
        IntentClassifier::Intent::UPTIME, IntentClassifier::Entity::NONE);
    add("check system run time", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has the core been running", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("uptime overview", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has rock os been powered", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("show uptime details", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has the software been running",
        IntentClassifier::Intent::UPTIME, IntentClassifier::Entity::NONE);
    add("uptime monitor", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has rock os been active today",
        IntentClassifier::Intent::UPTIME, IntentClassifier::Entity::NONE);
    add("check boot duration", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has the station been running",
        IntentClassifier::Intent::UPTIME, IntentClassifier::Entity::NONE);
    add("uptime tracker", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has rock os been running continuously",
        IntentClassifier::Intent::UPTIME, IntentClassifier::Entity::NONE);
    add("show system uptime report", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has the shell been active", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("uptime diagnostic", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has rock os been executing", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("check system uptime status", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("how long has the terminal been up", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("uptime metric check", IntentClassifier::Intent::UPTIME,
        IntentClassifier::Entity::NONE);
    add("open calculator", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::CALCULATOR);
    add("launch the browser", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::BROWSER);
    add("start terminal", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TERMINAL);
    add("open matrix", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MATRIX);
    add("launch rock ai", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::ROCK_AI);
    add("open tyrant", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TYRANT);
    add("start settings", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::SETTINGS);
    add("open music player", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("can you open the calculator?", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::CALCULATOR);
    add("yo launch calculator", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::CALCULATOR);
    add("bring up calculator", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::CALCULATOR);
    add("i need the calculator", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::CALCULATOR);
    add("start the calculator for me", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::CALCULATOR);
    add("calculator", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::CALCULATOR);
    add("hey rock ai, fire up the calculator",
        IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::CALCULATOR);
    add("could you launch calculator please",
        IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::CALCULATOR);
    add("open calc", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::CALCULATOR);
    add("can u launch terminal", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TERMINAL);
    add("open the browser pls", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::BROWSER);
    add("fire up the matrix", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MATRIX);
    add("start music player", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("launch settings app", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::SETTINGS);
    add("open tyrant program", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TYRANT);
    add("bring up rock ai interface", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::ROCK_AI);
    add("run calc", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::CALCULATOR);
    add("boot up browser", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::BROWSER);
    add("open up terminal", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TERMINAL);
    add("launch web browser", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::BROWSER);
    add("start the music app", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("open system settings", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::SETTINGS);
    add("fire up tyrant", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TYRANT);
    add("start rock ai", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::ROCK_AI);
    add("open the math tool", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::CALCULATOR);
    add("launch matrix screen", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MATRIX);
    add("open the shell", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TERMINAL);
    add("i want the browser", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::BROWSER);
    add("play music player", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("show settings", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::SETTINGS);
    add("open tyrant tool", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TYRANT);
    add("run rock ai", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::ROCK_AI);
    add("open calculation app", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::CALCULATOR);
    add("launch matrix app", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MATRIX);
    add("open command line", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TERMINAL);
    add("spin up browser", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::BROWSER);
    add("start music", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("open settings menu", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::SETTINGS);
    add("launch tyrant application", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TYRANT);
    add("open rock ai assistant", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::ROCK_AI);
    add("calc please", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::CALCULATOR);
    add("open internet browser", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::BROWSER);
    add("open command prompt", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TERMINAL);
    add("launch matrix visualizer", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MATRIX);
    add("start audio player", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("open configuration", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::SETTINGS);
    add("run tyrant", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TYRANT);
    add("open assistant", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::ROCK_AI);
    add("need calculator", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::CALCULATOR);
    add("fire up browser", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::BROWSER);
    add("open term", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TERMINAL);
    add("launch matrix window", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MATRIX);
    add("open songs", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("open preferences", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::SETTINGS);
    add("start tyrant", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TYRANT);
    add("bring up rock ai", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::ROCK_AI);
    add("launch calculator app", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::CALCULATOR);
    add("open net browser", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::BROWSER);
    add("fire up terminal", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TERMINAL);
    add("open matrix display", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MATRIX);
    add("launch music player app", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("open settings panel", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::SETTINGS);
    add("open tyrant app", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TYRANT);
    add("fire up rock ai", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::ROCK_AI);
    add("open arithmetic tool", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::CALCULATOR);
    add("start browser window", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::BROWSER);
    add("open shell window", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TERMINAL);
    add("run matrix", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MATRIX);
    add("open audio app", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("open system preferences", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::SETTINGS);
    add("launch tyrant tool", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TYRANT);
    add("open rock ai tool", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::ROCK_AI);
    add("calculator app please", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::CALCULATOR);
    add("open web client", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::BROWSER);
    add("launch bash terminal", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TERMINAL);
    add("open matrix app window", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MATRIX);
    add("open music application", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("open settings configuration", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::SETTINGS);
    add("open tyrant utility", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TYRANT);
    add("open rock ai companion", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::ROCK_AI);
    add("start calculator tool", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::CALCULATOR);
    add("open browser application", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::BROWSER);
    add("open system terminal", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TERMINAL);
    add("launch matrix interface", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MATRIX);
    add("open player", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("open configuration menu", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::SETTINGS);
    add("start tyrant application", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TYRANT);
    add("open rock ai system", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::ROCK_AI);
    add("open math application", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::CALCULATOR);
    add("open internet app", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::BROWSER);
    add("launch system shell", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TERMINAL);
    add("open matrix screen saver", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MATRIX);
    add("open audio player app", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("open settings window", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::SETTINGS);
    add("open tyrant manager", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::TYRANT);
    add("open rock ai module", IntentClassifier::Intent::OPEN_APP,
        IntentClassifier::Entity::ROCK_AI);
    add("close calculator", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("shut down the browser window", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("get rid of terminal", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("close the music player", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("exit settings", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("close matrix", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("shut down rock ai", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("close tyrant", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("close the calculator app", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("kill browser", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("quit terminal", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("exit matrix", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("stop music player", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("close settings window", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("terminate tyrant", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("close rock ai", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("shut calculator", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("close web browser", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("exit terminal", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("kill matrix window", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("close music app", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("shut down settings", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("exit tyrant app", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("stop rock ai", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("kill calculator", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("close browser app", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("shut terminal", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("close matrix application", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("exit music player", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("close settings panel", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("shut down tyrant", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("close rock ai assistant", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("exit calculator", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("shut browser", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("kill terminal", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("close matrix display", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("shut down music player", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("quit settings", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("close tyrant window", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("exit rock ai", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("close calc", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("exit browser", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("close term", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("kill matrix", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("close music", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("close settings app", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("kill tyrant", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("shut rock ai", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("get rid of calculator", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("close internet browser", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("close command line", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("shut matrix", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("close audio player", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("quit settings app", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("exit tyrant", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("close rock ai window", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("shut calculator window", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("close browser window", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("terminate terminal", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("close matrix visualizer", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("close music application", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("close settings menu", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("shut down tyrant window", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("terminate rock ai", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("stop calculator", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("terminate browser", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("close shell", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("stop matrix", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("close player", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("close system settings", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("stop tyrant", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("stop rock ai", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("kill calculator app", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("terminate browser application", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("close terminal window", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("kill matrix app", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("stop music app", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("terminate settings", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("kill tyrant application", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("terminate rock ai assistant", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("quit calculator", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("quit browser", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("quit terminal", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("quit matrix", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("quit music player", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("close configuration window", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("quit tyrant", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("quit rock ai", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("close calculator application", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("shut browser window", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("close bash terminal", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("close matrix application window",
        IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("close audio player window", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("terminate settings application",
        IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("close tyrant utility", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("close rock ai module", IntentClassifier::Intent::CLOSE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("minimize calculator", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("hide the browser", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("put terminal in the taskbar",
        IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("minimize that music player window",
        IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("minimize matrix", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("hide rock ai", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("minimize tyrant", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("minimize settings", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("hide calculator", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("minimize browser", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("put terminal away", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("minimize matrix window", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("hide music player", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("minimize settings window", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("hide tyrant", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("minimize rock ai", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("send calculator to taskbar", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("minimize web browser", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("hide terminal window", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("minimize matrix application",
        IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("put music player in taskbar",
        IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("hide settings", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("minimize tyrant application",
        IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("hide rock ai assistant", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("minimize calc", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("minimize browser window", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("minimize term", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("hide matrix", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("minimize music", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("minimize settings panel", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("hide tyrant window", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("minimize rock ai window", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("send calculator away", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("hide browser app", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("minimize terminal app", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("minimize matrix display", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("minimize music app", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("hide settings app", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("minimize tyrant app", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("minimize rock ai module", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("put calculator in taskbar", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("minimize internet browser", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("hide command line", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("minimize matrix visualizer", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("minimize audio player", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("hide configuration window", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("minimize tyrant utility", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("hide rock ai system", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("minimize calculator application",
        IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("minimize browser application",
        IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("hide terminal", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("minimize matrix application window",
        IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("minimize music application", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("minimize system settings", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("hide tyrant application", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("minimize rock ai interface", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("send calculator to dock", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("send browser to taskbar", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("send terminal to taskbar", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("send matrix to taskbar", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("send music player to taskbar",
        IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("send settings to taskbar", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("send tyrant to taskbar", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("send rock ai to taskbar", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("minimize the calculator window",
        IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("minimize the browser app", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("minimize the terminal window",
        IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("minimize the matrix window", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("minimize the music app window",
        IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("minimize the settings app", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("minimize the tyrant window", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("minimize the rock ai window",
        IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("iconify calculator", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("iconify browser", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("iconify terminal", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("iconify matrix", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("iconify music player", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("iconify settings", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("iconify tyrant", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("iconify rock ai", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("put calculator in background",
        IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::CALCULATOR);
    add("put browser in background", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::BROWSER);
    add("put terminal in background", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::TERMINAL);
    add("put matrix in background", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::MATRIX);
    add("put music player in background",
        IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::MUSIC_PLAYER);
    add("put settings in background", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::SETTINGS);
    add("put tyrant in background", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::TYRANT);
    add("put rock ai in background", IntentClassifier::Intent::MINIMIZE_WINDOW,
        IntentClassifier::Entity::ROCK_AI);
    add("help", IntentClassifier::Intent::HELP, IntentClassifier::Entity::NONE);
    add("what can you do", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what commands do you know", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("how can you help me", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("show me what you can do", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what can I ask you", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("give me some help", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("help me please", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what are your features", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("how do you work", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("show help", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("i need assistance", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what capabilities do you have", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("can you help", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("guide me", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what options are available", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("assist me", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what instructions can i use", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("tell me what you can do", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("help menu", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what functions do you support", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("i need help with rock os", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("how do i use rock ai", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("show available commands", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what are my options", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("give me assistance", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what can you assist with", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("help guide", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("how can rock ai help me", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what commands are supported", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("show me commands", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("i need some assistance", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what tasks can you perform", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("help information", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what can i ask rock ai", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("show me help", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("how do i interact with you", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what are your instructions", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("assistance please", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what features do you offer", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("can you show me help", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what is rock ai capable of", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("help me out", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what can you do for me", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("display help", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("how do you operate", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what commands do you accept", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("give me a hand", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what can i do here", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("help and support", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what services do you provide", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("show me assistant features", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("how can i get help", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what are your main functions", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("support please", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what queries can you answer", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("help documentation", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what is your purpose", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("show assistant help", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("how do i use this assistant", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what commands are available", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("give me command list", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what can you manage", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("help system", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what can i ask you to do", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("show me instruction manual", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("how do i control rock os", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what actions can you take", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("help me learn", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what are all your commands", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("display commands", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("how can rock os assist", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what skills do you have", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("give me help instructions", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what can we do", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("help overview", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what commands work here", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("show me help options", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("how do i get started", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what are your abilities", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("give me a list of commands", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what can you handle", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("help details", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what can i say to you", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("show me quick help", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("how do i interact with rock os", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what commands can i type", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("help summary", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what can rock ai do for me", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("show assistance", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("how can you assist me today", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what are your core features", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("give me general help", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what can you execute", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("help reference", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("what can i prompt you with", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
    add("show me the help menu", IntentClassifier::Intent::HELP,
        IntentClassifier::Entity::NONE);
  }
};
