#pragma once

#include "requestmessage.h"

void path_parse(const RequestMessage& request);

void request_help(const RequestMessage& request);
void request_image(const RequestMessage& request);
void request_audio(const RequestMessage& request);
void request_stop_audio(const RequestMessage& request);
void request_mic(const RequestMessage& request);
void request_motor(const RequestMessage& request);