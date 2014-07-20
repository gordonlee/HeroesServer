#pragma once

#include <stdio.h>

namespace VCore
{

	enum LogLevel
	{
		DEBUG = 0,
		INFO,
		WARN,
		FATAL,
		SEND,
		RECV
	};

	template <typename T>
	class Logger
	{
	public:
		static void Debug(char* message)
		{
			Log(LogLevel::DEBUG, message);
		}

		static void Info(char* message)
		{
			Log(LogLevel::INFO, message);
		}

		static void Warn(char* message)
		{
			Log(LogLevel::WARN, message);
		}

		static void Fatal(char* message)
		{
			Log(LogLevel::FATAL, message);
		}

		static void Send(char* message)
		{
			Log(LogLevel::SEND, message);
		}

		static void Recieve(char* message)
		{
			Log(LogLevel::RECV, message);
		}

	private:

		enum LOG_COLOR{
			BLACK,
			D_BLUE,
			D_GREEN,
			D_SKYBLUE,
			D_RED,
			D_VIOLET,
			D_YELLOW,
			GRAY,
			D_GRAY,
			BLUE,
			GREEN,
			SKYBLUE,
			RED,
			VIOLET,
			YELLOW,
			WHITE,
		};

		static void SetColor(int backcolor, int fontcolor)
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), backcolor * 16 + fontcolor);
		}

		static void Log(LogLevel level, char* message)
		{
			switch (level)
			{
			case LogLevel::DEBUG:
				SetColor(LOG_COLOR::BLACK, LOG_COLOR::WHITE);
				break;
			case LogLevel::INFO:
				SetColor(LOG_COLOR::BLACK, LOG_COLOR::GREEN);
				break;
			case LogLevel::WARN:
				SetColor(LOG_COLOR::BLACK, LOG_COLOR::YELLOW);
				break;
			case LogLevel::FATAL:
				SetColor(LOG_COLOR::BLACK, LOG_COLOR::RED);
				break;
			case LogLevel::SEND:
				SetColor(LOG_COLOR::BLACK, LOG_COLOR::BLUE);
				break;
			case LogLevel::RECV:
				SetColor(LOG_COLOR::BLACK, LOG_COLOR::VIOLET);
				break;
			}


			if (level == LogLevel::SEND || level == LogLevel::RECV)
			{
				printf_s("%s", message);
			}
			else
			{
				printf_s("[%s] - %s\n", typeid(T).name(), message);
			}

			SetColor(LOG_COLOR::BLACK, LOG_COLOR::GRAY);
		}

		static Logger* instance_;


	};

	template <typename T> Logger<T>* Logger<T>::instance_;

}