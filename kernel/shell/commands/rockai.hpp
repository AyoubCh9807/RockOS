#include "../../ai/rock_ai.hpp"
#include "../../core/rtc.hpp"
#include "icommand.hpp"

class AICommand : public ICommand {
private:
  RockAI &ai;

public:
  AICommand(RockAI &rock_ai) : ai(rock_ai) {}

  CommandResult execute(int argc, char **argv) override {
    char req[1024];
    StringUtils::join(req, sizeof(req), argc, argv, " ");
    ai.send_request(req);

    const AIResponse &response = ai.get_response();

    if (!response.succeeded())
      return CommandResult(response.text().c_str(), Colors::RED);

    if (response.type() == AIResponseType::TEXT)
      return CommandResult(response.text().c_str(), Colors::WHITE);

    return handle_intent(response);
  }

  const char *name() const { return "rockai"; }

private:
  CommandResult handle_intent(const AIResponse &response) {

    char message[1024];

    switch (response.get_intent()) {

    case IntentClassifier::Intent::MEMORY_USAGE: {
      const u32 used = heap.get_used();

      if (used < 1024) {
        StringUtils::snprintf(
            message, sizeof(message),
            "Lemme check that for ya... Oh! Rock OS is currently using %d B.",
            (int)used);
      } else if (used < 1024 * 1024) {
        const u32 used_kb = used / 1024;

        StringUtils::snprintf(
            message, sizeof(message),
            "Lemme check that for ya... Oh! Rock OS is currently using %d KB.",
            (int)used_kb);
      } else {
        const u32 used_mb = used / (1024 * 1024);

        StringUtils::snprintf(
            message, sizeof(message),
            "Lemme check that for ya... Oh! Rock OS is currently using %d MB.",
            (int)used_mb);
      }

      break;
    }

    case IntentClassifier::Intent::TIME: {
      char time[64];

      RTC::get_full_time_into(time, sizeof(time));

      StringUtils::snprintf(
          message, sizeof(message),
          "Let ME check the clock your highness... It's %s right now.", time);

      break;
    }

    case IntentClassifier::Intent::UPTIME: {
      char uptime[64];

      Timer::get_formatted_time_into(uptime, sizeof(uptime));

      StringUtils::snprintf(message, sizeof(message),
                            "You have been exhausting me for %s NONSTOP.",
                            uptime);
      break;
    }

    default:
      break;
    }

    return CommandResult(message, Colors::WHITE);
  }
};
