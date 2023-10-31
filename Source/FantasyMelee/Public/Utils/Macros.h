// Copyright AB. All Rights Reserved.

#pragma once

#define SCREEN_LOG(Message, Duration) GEngine->AddOnScreenDebugMessage(-1, Duration, FColor::Cyan, Message);
#define SCREEN_LOG_KEYED(Message, Duration, Key) GEngine->AddOnScreenDebugMessage(Key, Duration, FColor::Cyan, Message);
