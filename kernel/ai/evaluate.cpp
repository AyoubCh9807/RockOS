#include "embedding.hpp"
#include "intent_classifier.hpp"
#include "model_io.hpp"
#include "positional_encoder.hpp"
#include "tokenizer.hpp"
#include "transform_layer.hpp"
#include "transformer_input.hpp"
#include "vocabulary.hpp"

#include <iostream>

static IntentClassifier::Intent predict(
    const char *text,
    Tokenizer &tokenizer,
    Embedding &embedding,
    PositionalEncoder &pos_encoder,
    Vector<TransformerLayer *> &layers,
    IntentClassifier &classifier) {

  String phrase(text);

  Vector<Token> &tokens =
      tokenizer.tokenize(phrase);

  if (tokens.size() == 0)
    return IntentClassifier::Intent::UNKNOWN;

  TransformerInput input(
      embedding,
      pos_encoder);

  input.build(tokens);

  Vector<Vector<float>> sequence =
      input.get();

  for (int i = 0; i < layers.size(); i++)
    sequence =
        layers[i]->forward(sequence);

  return classifier.predict(sequence);
}

static const char *intent_name(
    IntentClassifier::Intent intent) {

  switch (intent) {

  case IntentClassifier::Intent::GREETING:
    return "GREETING";

  case IntentClassifier::Intent::MEMORY_USAGE:
    return "MEMORY_USAGE";

  case IntentClassifier::Intent::TIME:
    return "TIME";

  case IntentClassifier::Intent::UPTIME:
    return "UPTIME";

  case IntentClassifier::Intent::OPEN_APP:
    return "OPEN_APP";

  case IntentClassifier::Intent::CLOSE_WINDOW:
    return "CLOSE_WINDOW";

  case IntentClassifier::Intent::MINIMIZE_WINDOW:
    return "MINIMIZE_WINDOW";

  case IntentClassifier::Intent::HELP:
    return "HELP";

  default:
    return "UNKNOWN";
  }
}

static void respond(
    IntentClassifier::Intent intent) {

  switch (intent) {

  case IntentClassifier::Intent::GREETING:
    std::cout
        << "Rock AI: Yo! What's up? 🤘\n";
    break;

  case IntentClassifier::Intent::MEMORY_USAGE:
    std::cout
        << "Rock AI: You want to check memory usage.\n";
    break;

  case IntentClassifier::Intent::TIME:
    std::cout
        << "Rock AI: You want to know the time.\n";
    break;

  case IntentClassifier::Intent::UPTIME:
    std::cout
        << "Rock AI: You want to know how long Rock OS has been running.\n";
    break;

  case IntentClassifier::Intent::OPEN_APP:
    std::cout
        << "Rock AI: You want to open an app.\n";
    break;

  case IntentClassifier::Intent::CLOSE_WINDOW:
    std::cout
        << "Rock AI: You want to close a window.\n";
    break;

  case IntentClassifier::Intent::MINIMIZE_WINDOW:
    std::cout
        << "Rock AI: You want to minimize a window.\n";
    break;

  case IntentClassifier::Intent::HELP:
    std::cout
        << "Rock AI: You want some help.\n";
    break;

  default:
    std::cout
        << "Rock AI: I don't understand that yet.\n";
    break;
  }
}

int main() {

  std::cout
      << "Loading Rock AI evaluation environment...\n";

  Vocabulary vocab;

  String words[] = {
      String("hello"),
      String("hi"),
      String("hey"),
      String("yo"),
      String("rock"),
      String("good"),
      String("morning"),
      String("evening"),
      String("what"),
      String("up"),
      String("how"),
      String("are"),
      String("you"),
      String("on"),
      String("os"),
      String("damian"),
      String("wake"),
      String("ready"),
      String("to"),
      String("shred"),
      String("greetings"),
      String("there"),
      String("afternoon"),
      String("howdy"),
      String("sup"),
      String("assistant"),
      String("system"),
      String("garrick"),
      String("tyrant"),
      String("flamie"),
      String("salutations"),
      String("is"),
      String("anyone"),
      String("friend"),
      String("buddy"),
      String("team"),
      String("top"),
      String("of"),
      String("the"),
      String("well"),
      String("hiya"),
      String("traveler"),
      String("program"),
      String("computer"),
      String("machine"),
      String("rise"),
      String("shine"),
      String("again"),
      String("nice"),
      String("meet"),
      String("do"),
      String("reporting"),
      String("duty"),
      String("let"),
      String("roll"),
      String("digital"),
      String("world"),
      String("folks"),
      String("everyone"),
      String("cracking"),
      String("things"),
      String("happening"),
      String("long"),
      String("time"),
      String("no"),
      String("see"),
      String("look"),
      String("who"),
      String("gorgeous"),
      String("boss"),
      String("chief"),
      String("night"),
      String("ai"),
      String("partner"),
      String("goes"),
      String("home"),
      String("kernel"),
      String("terminal"),
      String("jam"),
      String("get"),
      String("loud"),
      String("tiger"),
      String("crocodile"),
      String("lion"),
      String("dragon"),
      String("shaking"),
      String("fare"),
      String("met"),
      String("ahoy"),
      String("show"),
      String("ram"),
      String("am"),
      String("using"),
      String("free"),
      String("check"),
      String("usage"),
      String("low"),
      String("beast"),
      String("eating"),
      String("status"),
      String("left"),
      String("current"),
      String("consumption"),
      String("display"),
      String("stats"),
      String("burning"),
      String("full"),
      String("have"),
      String("enough"),
      String("megabytes"),
      String("gigabytes"),
      String("right"),
      String("now"),
      String("can"),
      String("please"),
      String("need"),
      String("heavy"),
      String("load"),
      String("overloaded"),
      String("consumed"),
      String("report"),
      String("info"),
      String("taken"),
      String("occupied"),
      String("available"),
      String("headroom"),
      String("maxed"),
      String("out"),
      String("footprint"),
      String("allocation"),
      String("breakdown"),
      String("track"),
      String("monitor"),
      String("hogged"),
      String("which"),
      String("apps"),
      String("tank"),
      String("total"),
      String("take"),
      String("active"),
      String("inactive"),
      String("clock"),
      String("tell"),
      String("saying"),
      String("hour"),
      String("minute"),
      String("exact"),
      String("late"),
      String("early"),
      String("read"),
      String("timestamp"),
      String("mark"),
      String("quick"),
      String("precise"),
      String("today"),
      String("uptime"),
      String("running"),
      String("booted"),
      String("boot"),
      String("reboot"),
      String("awake"),
      String("since"),
      String("days"),
      String("amp"),
      String("powered"),
      String("online"),
      String("session"),
      String("lasted"),
      String("duration"),
      String("concert"),
      String("bootup"),
      String("seconds"),
      String("minutes"),
      String("counter"),
      String("live"),
      String("operational"),
      String("open"),
      String("launch"),
      String("start"),
      String("bring"),
      String("browser"),
      String("paint"),
      String("matrix"),
      String("calculator"),
      String("metronome"),
      String("lyrics"),
      String("mixer"),
      String("store"),
      String("vinyl"),
      String("microphone"),
      String("drum"),
      String("recorder"),
      String("equalizer"),
      String("tuner"),
      String("radio"),
      String("playlist"),
      String("fire"),
      String("execute"),
      String("spin"),
      String("music"),
      String("close"),
      String("exit"),
      String("quit"),
      String("shut"),
      String("rid"),
      String("terminate"),
      String("destroy"),
      String("dismiss"),
      String("stop"),
      String("end"),
      String("down"),
      String("drop"),
      String("force"),
      String("cut"),
      String("wrap"),
      String("panel"),
      String("screen"),
      String("graveyard"),
      String("abort"),
      String("halt"),
      String("minimize"),
      String("hide"),
      String("put"),
      String("away"),
      String("taskbar"),
      String("stage"),
      String("collapse"),
      String("iconify"),
      String("dock"),
      String("background"),
      String("shove"),
      String("stash"),
      String("park"),
      String("keep"),
      String("disappear"),
      String("temporarily"),
      String("sight"),
      String("help"),
      String("assistance"),
      String("guide"),
      String("options"),
      String("say"),
      String("accept"),
      String("list"),
      String("menu"),
      String("work"),
      String("explain"),
      String("instructions"),
      String("lost"),
      String("manual"),
      String("hand"),
      String("capabilities"),
      String("features"),
      String("operate"),
      String("tutorial"),
      String("started"),
      String("protocol"),
      String("actions"),
      String("perform"),
      String("purpose"),
      String("reference"),
      String("ropes"),
      String("teach")
  };

  constexpr int WORD_COUNT =
      sizeof(words) / sizeof(words[0]);

  for (int i = 0; i < WORD_COUNT; i++)
    vocab.add(words[i]);

  Embedding embedding(vocab.size());

  PositionalEncoder pos_encoder;

  Tokenizer tokenizer(vocab);

  TransformerLayer layer;

  Vector<TransformerLayer *> layers;

  layers.push_back(&layer);

  IntentClassifier classifier;

  constexpr const char *MODEL_FILE =
      "rock_ai.model";

  if (!ModelIO::load(
          MODEL_FILE,
          embedding,
          layers,
          classifier)) {

    std::cout
        << "ERROR: Could not load Rock AI model.\n";

    std::cout
        << "Train the model first with main.cpp.\n";

    return 1;
  }

  std::cout
      << "Rock AI model loaded successfully.\n";

  std::cout
      << "\nRock AI evaluation mode.\n"
      << "Type 'quit' to exit.\n\n";

  char input[256];

  while (true) {

    std::cout << "You: ";

    std::cin.getline(
        input,
        sizeof(input));

    if (input[0] == '\0')
      continue;

    if (input[0] == 'q' &&
        input[1] == 'u' &&
        input[2] == 'i' &&
        input[3] == 't' &&
        input[4] == '\0') {

      break;
    }

    IntentClassifier::Intent prediction =
        predict(
            input,
            tokenizer,
            embedding,
            pos_encoder,
            layers,
            classifier);

    std::cout
        << "Intent: "
        << intent_name(prediction)
        << " ("
        << static_cast<int>(prediction)
        << ")\n";

    respond(prediction);

    std::cout << "\n";
  }

  std::cout
      << "\nRock AI evaluation finished. 🤘\n";

  return 0;
}

