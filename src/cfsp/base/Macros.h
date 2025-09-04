#pragma once

#ifdef CFSPEXP
#define CFSP_API __declspec(dllexport)
#else
#define CFSP_API __declspec(dllimport)
#endif
