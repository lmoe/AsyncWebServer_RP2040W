/****************************************************************************************************************************
  AsyncWebResponseImpl_RP2040W.h
  
  For RP2040W with CYW43439 WiFi
  
  AsyncWebServer_RP2040W is a library for the RP2040W with CYW43439 WiFi
  
  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_RP2040W
  Licensed under GPLv3 license
 
  Version: 1.0.0
  
  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      13/08/2022 Initial coding for RP2040W with CYW43439 WiFi
 *****************************************************************************************************************************/

#pragma once

#ifndef RP2040W_ASYNCWEBSERVERRESPONSEIMPL_H_
#define RP2040W_ASYNCWEBSERVERRESPONSEIMPL_H_

#ifdef Arduino_h
  // arduino is not compatible with std::vector
  #undef min
  #undef max
#endif

#include <vector>
// It is possible to restore these defines, but one can use _min and _max instead. Or std::min, std::max.

class AsyncBasicResponse: public AsyncWebServerResponse 
{
  private:
    String _content;
    
  public:
    AsyncBasicResponse(int code, const String& contentType = String(), const String& content = String());
    void _respond(AsyncWebServerRequest *request);
    
    size_t _ack(AsyncWebServerRequest *request, size_t len, uint32_t time);
    
    bool _sourceValid() const 
    {
      return true;
    }
};

class AsyncAbstractResponse: public AsyncWebServerResponse 
{
  private:
    String _head;
    // Data is inserted into cache at begin().
    // This is inefficient with vector, but if we use some other container,
    // we won't be able to access it as contiguous array of bytes when reading from it,
    // so by gaining performance in one place, we'll lose it in another.
    std::vector<uint8_t> _cache;
    size_t _readDataFromCacheOrContent(uint8_t* data, const size_t len);
    size_t _fillBufferAndProcessTemplates(uint8_t* buf, size_t maxLen);
    
  protected:
    AwsTemplateProcessor _callback;
    
  public:
    AsyncAbstractResponse(AwsTemplateProcessor callback = nullptr);
    void _respond(AsyncWebServerRequest *request);
    size_t _ack(AsyncWebServerRequest *request, size_t len, uint32_t time);
    
    bool _sourceValid() const 
    {
      return false;
    }
    
    virtual size_t _fillBuffer(uint8_t *buf __attribute__((unused)), size_t maxLen __attribute__((unused))) 
    {
      return 0;
    }
};

#ifndef TEMPLATE_PLACEHOLDER
  #define TEMPLATE_PLACEHOLDER '%'
#endif

#define TEMPLATE_PARAM_NAME_LENGTH 32

class AsyncStreamResponse: public AsyncAbstractResponse 
{
  private:
    Stream *_content;
    
  public:
    AsyncStreamResponse(Stream &stream, const String& contentType, size_t len, AwsTemplateProcessor callback = nullptr);
    
    bool _sourceValid() const 
    {
      return !!(_content);
    }
    
    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
};

class AsyncCallbackResponse: public AsyncAbstractResponse 
{
  private:
    AwsResponseFiller _content;
    size_t _filledLength;
    
  public:
    AsyncCallbackResponse(const String& contentType, size_t len, AwsResponseFiller callback, AwsTemplateProcessor templateCallback = nullptr);
    
    bool _sourceValid() const 
    {
      return !!(_content);
    }
    
    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
};

class AsyncChunkedResponse: public AsyncAbstractResponse 
{
  private:
    AwsResponseFiller _content;
    size_t _filledLength;
    
  public:
    AsyncChunkedResponse(const String& contentType, AwsResponseFiller callback, AwsTemplateProcessor templateCallback = nullptr);
    
    bool _sourceValid() const 
    {
      return !!(_content);
    }
    
    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
};

class cbuf;

class AsyncResponseStream: public AsyncAbstractResponse, public Print 
{
  private:
    cbuf *_content;
    
  public:
    AsyncResponseStream(const String& contentType, size_t bufferSize);
    ~AsyncResponseStream();
    
    bool _sourceValid() const 
    {
      return (_state < RESPONSE_END);
    }
    
    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
    size_t write(const uint8_t *data, size_t len);
    size_t write(uint8_t data);
    using Print::write;
};

#endif /* RP2040W_ASYNCWEBSERVERRESPONSEIMPL_H_ */