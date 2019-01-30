#pragma once

#ifdef UTS_FRAMEWORK_EXPORTS
#define UTS_FRAMEWORK_API __declspec(dllexport)
#else
#define UTS_FRAMEWORK_API __declspec(dllimport)
#endif
