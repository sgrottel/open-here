// Open Here
// Copyright 2022 SGrottel (https://www.sgrottel.de)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
//
#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <mutex>

namespace openhere
{
namespace toolbox
{

	class LogFile
	{
	public:
		class Line {
		private:
			Line();
			std::wstringstream m_stream;
		public:
			~Line();
			Line(Line&&) = delete;
			Line(Line const&) = delete;
			Line& operator=(Line&&) = delete;
			Line& operator=(Line const&) = delete;

			inline Line& operator<<(const char* s)
			{
				m_stream << s;
				return *this;
			}
			inline Line& operator<<(const std::string& s)
			{
				m_stream << s.c_str();
				return *this;
			}
			template<class T>
			inline Line& operator<<(T s)
			{
				m_stream << s;
				return *this;
			}

			friend class LogFile;
		};

		static inline Line Write() { return Line{}; }

		static inline void Write(const char* str) { Write() << str; }
		static inline void Write(const wchar_t* str) { Write() << str; }
		static inline void Write(const std::string& str) { Write() << str; }
		static inline void Write(const std::wstring& str) { Write() << str; }

		static inline bool IsEnabled() { return m_enabled; }
		static void Enable(bool enable);

	private:

		static inline void Write(const std::wstringstream& stream);

		static std::mutex m_writeLock;
		static bool m_enabled;
		static std::vector<std::wstring> m_lineBuf;

		friend class Line;
	};

}
}
