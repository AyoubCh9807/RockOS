#pragma once

#include "../containers/string.hpp"
#include "../containers/vector.hpp"

enum class AIResponseType { ACTION, TEXT };

class AIRequest {
private:
  String request_text;

public:
  AIRequest() = default;

  AIRequest(const char *text) : request_text(text) {}

  const String &text() const { return request_text; }
};

class AIResponse {
protected:
  String response_text;
  AIResponseType response_type;
  bool success;

public:
  AIResponse() : response_type(AIResponseType::TEXT), success(false) {}

  AIResponse(const char *text, AIResponseType type, bool successful)
      : response_text(text), response_type(type), success(successful) {}

  const String &text() const { return response_text; }

  AIResponseType type() const { return response_type; }

  bool succeeded() const { return success; }
};

class AITextResponse : public AIResponse {
public:
  AITextResponse(const char *text, bool successful = true)
      : AIResponse(text, AIResponseType::TEXT, successful) {}
};

class AIActionResponse : public AIResponse {
public:
  AIActionResponse(const char *text, bool successful = true)
      : AIResponse(text, AIResponseType::ACTION, successful) {}
};

class RockAI {

private:
  AIRequest current_request;
  AIResponse current_response;

  AIResponse process_request(const AIRequest &request) {
    if (request.text() == "hello") {
      return AITextResponse("Rock OS AI online. What are we breaking today?");
    }

    if (request.text() == "who are you") {
      return AITextResponse("I'm Rock AI. Small brain. Loud personality.");
    }

    if (request.text() == "help") {
      return AITextResponse("Try: hello, who are you, help");
    }

    return AITextResponse("I don't understand that yet.", false);
  }

public:
  RockAI() = default;

  void send_request(const AIRequest &request) {
    current_request = request;
    current_response = process_request(current_request);
  }

  const AIResponse &get_response() const { return current_response; }
}; 
