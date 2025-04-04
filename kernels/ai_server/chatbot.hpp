/**
 * @file chatbot.hpp
 * @brief Kernel for AI Model Server Chatbot
 * @version 0.1
 * @date 2025-04-04
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <VX/vx.h>

#define DEFAULT_MODEL "gpt-4o-mini"
#define SERVER_URL "http://localhost:8000"
#define API_KEY "hardcoded-api-key"

class RemoteModelClient
{
private:
    // Helper function for non-streaming response
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
    {
        size_t totalSize = size * nmemb;
        ((std::string *)userp)->append((char *)contents, totalSize);
        return totalSize;
    }

public:
    // kernel function (non-streaming)
    vx_status AiServerQuery(const std::string &input_text, std::string &output_text, const std::string &api_path)
    {
        CURL *curl = curl_easy_init();
        if (!curl)
            return VX_FAILURE;

        nlohmann::json request_json = {
            {"model", DEFAULT_MODEL},
            {"messages", {{{"role", "user"}, {"content", input_text}}}},
            {"max_tokens", 100},
            {"stream", false}};

        std::string request_payload = request_json.dump();
        std::string response_string;
        std::string api_url = std::string(SERVER_URL) + api_path;

        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("Authorization: Bearer " + std::string(API_KEY)).c_str());

        curl_easy_setopt(curl, CURLOPT_URL, api_url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_payload.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK)
            return VX_FAILURE;

        auto json_response = nlohmann::json::parse(response_string);
        output_text = json_response["choices"][0]["message"]["content"];

        return VX_SUCCESS;
    }

    // kernel function (streaming)
    vx_status AiServerQueryStream(const std::string &input_text, std::string &output_text, const std::string &api_path)
    {
        CURL *curl = curl_easy_init();
        if (!curl)
            return VX_FAILURE;

        nlohmann::json request_json = {
            {"model", DEFAULT_MODEL},
            {"messages", {{{"role", "user"}, {"content", input_text}}}},
            {"max_tokens", 100},
            {"stream", true}};

        std::string request_payload = request_json.dump();
        std::string response_chunk;
        std::string api_url = std::string(SERVER_URL) + api_path;

        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("Authorization: Bearer " + std::string(API_KEY)).c_str());

        curl_easy_setopt(curl, CURLOPT_URL, api_url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_payload.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_chunk);

        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK)
            return VX_FAILURE;

        // Just return raw streamed response (newline-delimited JSON chunks)
        output_text = response_chunk;
        return VX_SUCCESS;
    }
};
