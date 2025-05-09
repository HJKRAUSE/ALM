#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <limits>
#include <functional>
#include <cctype>
#include "Date.h"

namespace ALM {

	class UI {
	public:
		class Color {
		public:
			inline static constexpr const char* Reset = "\033[0m";
			inline static constexpr const char* Red = "\033[31m";
			inline static constexpr const char* Green = "\033[32m";
			inline static constexpr const char* Yellow = "\033[33m";
			inline static constexpr const char* Cyan = "\033[36m";
			inline static constexpr const char* Bold = "\033[1m";
			inline static constexpr const char* BoldCyan = "\033[1;36m";
			inline static constexpr const char* White = "\033[37m";
			inline static constexpr const char* Gray = "\033[2;37m";
			inline static constexpr const char* None = "";
		};
		enum class Verbosity {
			Silent = 0,
			Error = 1,
			Warn = 2,
			Info = 3,
			Debug = 4
		};
		// Generic ask<T>
		template<typename T>
		static T ask(const std::string& prompt, const T& default_value) {
			while (true) {
				std::cout  
					<< (useColor ? Color::Cyan : "")
					<< prompt << " [default: " << default_value << "]: "
					<< (useColor ? Color::Reset : "");

				std::string input;
				std::getline(std::cin, input);

				if (input.empty()) {
					return default_value;
				}

				std::istringstream ss(input);
				T value;
				if (ss >> value) {
					return value;
				}

				std::cout << "Invalid input. Please try again.\n";
			}
		}

		// Specialization for std::string
		template<>
		static std::string ask<std::string>(const std::string& prompt, const std::string& default_value) {
			std::cout 
				<< (useColor ? Color::Cyan : Color::None)
				<< prompt << " [default: " << default_value << "]: "
				<< (useColor ? Color::Reset : Color::None);

			std::string input;
			std::getline(std::cin, input);
			return input.empty() ? default_value : input;
		}

		// Ask for Yes/No input
		static bool askYesNo(const std::string& prompt, bool default_value = true) {
			std::string default_str = default_value ? "Y" : "N";
			while (true) {
				std::cout 
					<< (useColor ? Color::Cyan : Color::None)
					<< prompt << " [Y/N] (default: " << default_str << "): "
					<< (useColor ? Color::Reset : Color::None);
				std::string input;
				std::getline(std::cin, input);
				if (input.empty()) return default_value;

				char c = std::tolower(input[0]);
				if (c == 'y') return true;
				if (c == 'n') return false;

				std::cout << "Invalid input. Please enter Y or N.\n";
			}
		}

		// Print a message
		static void print(const std::string& msg) {
			if (verbosity_ < Verbosity::Info) return;
			std::cout << msg << "\n";
		}

		static void debugPrint(const std::string& msg) {
			if (verbosity_ < Verbosity::Debug) return;
			std::cout << (useColor ? Color::Gray : Color::None)
				<< "[DEBUG] " << msg
				<< (useColor ? Color::Reset : Color::None) << "\n";
		}

		// Print a section header
		static void section(const std::string& title) {
			if (verbosity_ < Verbosity::Info) return;
			std::cout 
				<< (useColor ? Color::BoldCyan : Color::None)
				<< "\n=== " << title << " ===\n"
				<< (useColor ? Color::Reset : Color::None);
		}

		// Print a warning
		static void warn(const std::string& msg) {
			if (verbosity_ < Verbosity::Warn) return;
			std::cout 
				<< (useColor ? Color::Yellow : Color::None)
				<< "[WARNING] " << msg << "\n"
				<< (useColor ? Color::Reset : Color::None);
		}

		// Print an error
		static void error(const std::string& msg) {
			if (verbosity_ < Verbosity::Error) return;
			std::cout 
				<< (useColor ? Color::Red : Color::None)
				<< "[ERROR] " << msg <<  "\n"
				<< (useColor ? Color::Reset : Color::None);
		}

		static void clearScreen(bool hard = true) {
			if (hard && use_color_) {
				std::cout << "\033[2J\033[H";
			}
			else {
				std::cout << std::string(50, '\n');
			}
		}
		
		static void useColor(bool use_color = true) {
			use_color_ = use_color;
		}

		static void setVerbosity(Verbosity verbosity = Verbosity::Info) {
			verbosity_ = verbosity;
		}
	private:

		static inline bool use_color_ = false;
		static inline Verbosity verbosity_ = Verbosity::Info;
	};

}  // namespace ALM
