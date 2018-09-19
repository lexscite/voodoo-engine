#ifndef NIXIE_LOG_H_
#define NIXIE_LOG_H_

#pragma once

// Comment this to disable Windows native debug console window
#ifndef NIXIE_LOG_CONSOLE_ENABLED
#define NIXIE_LOG_CONSOLE_ENABLED
#endif


namespace nixie
{
#ifdef NIXIE_LOG_CONSOLE_ENABLED
	class ConsoleBuffer : public std::streambuf {
	public:
		ConsoleBuffer() {
			setp(0, 0);
		}

		virtual int_type overflow(int_type c = traits_type::eof()) {
			return fputc(c, stdout) == EOF ? traits_type::eof() : c;
		}
	};
#endif

	class Logger final
	{
	public:
		~Logger();

		static void Write(std::string s);

	private:
		Logger();

		static Logger& Get();

#ifdef NIXIE_LOG_CONSOLE_ENABLED
		static void WriteToConsole(std::string s);
#endif

	private:
		std::ofstream fs_;

#ifdef NIXIE_LOG_CONSOLE_ENABLED
		ConsoleBuffer console_buffer_;
		std::streambuf* cout_buffer_;
		std::streambuf* cerr_buffer_;
#endif
	};
}

#endif