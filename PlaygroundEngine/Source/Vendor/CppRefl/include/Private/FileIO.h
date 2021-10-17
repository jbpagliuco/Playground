#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include <assert.h>

#include "ReflectionClass.h"

namespace refl
{
	class FileIO
	{
	public:
		FileIO(const std::string& filename, bool read);

		bool IsOpen()const;
		void Close();

		bool IsReading()const;
		bool IsWriting()const;

		void operator>>(std::string& value)
		{
			size_t size = value.size();
			this->operator>>(size);

			if (mRead) {
				char* buf = (char*)alloca(size + 1);
				ReadBytes(buf, size);
				buf[size] = 0;
				value = std::string(buf);
			}
			else {
				WriteBytes(value.c_str(), size);
			}
		}

		template <typename T>
		void operator>>(T& value)
		{
			if (mRead) {
				Read(value);
			}
			else {
				Write(value);
			}
		}

		template <typename T>
		void SerializeVector(std::vector<T>& vector)
		{
			size_t size = vector.size();
			this->operator>>(size);

			if (mRead) {
				for (int i = 0; i < size; ++i) {
					T element;
					this->operator>>(element);
					vector.push_back(element);
				}
			}
			else {
				for (int i = 0; i < size; ++i) {
					T& element = vector[i];
					this->operator>>(element);
				}
			}
		}

		template <typename T>
		void SerializeReflVector(std::vector<T>& vector, const Registry& registry)
		{
			size_t size = vector.size();
			this->operator>>(size);

			if (mRead) {
				for (int i = 0; i < size; ++i) {
					T element;
					element.Serialize(*this, registry);
					vector.push_back(element);
				}
			}
			else {
				for (int i = 0; i < size; ++i) {
					T& element = vector[i];
					element.Serialize(*this, registry);
				}
			}
		}

		// Helper functions
		template <typename Key, typename Value>
		void SerializeMap(std::map<Key, Value>& map)
		{
			size_t size = map.size();
			this->operator>>(size);

			if (mRead) {
				for (int i = 0; i < size; ++i) {
					Key key;
					this->operator>>(key);

					Value value;
					this->operator>>(value);

					map[key] = value;
				}
			}
			else {
				for (auto& iter : map) {
					auto key = iter.first;
					auto value = iter.second;
					this->operator>>(key);
					this->operator>>(value);
				}
			}
		}

		template <typename Key, typename ReflValue>
		void SerializeReflMap(std::map<Key, ReflValue>& map, const Registry& registry)
		{
			size_t size = map.size();
			this->operator>>(size);

			if (mRead) {
				for (int i = 0; i < size; ++i) {
					Key key;
					this->operator>>(key);

					ReflValue value;
					value.Serialize(*this, registry);

					map[key] = value;
				}
			}
			else {
				for (auto& iter : map) {
					auto key = iter.first;
					this->operator>>(key);
					iter.second.Serialize(*this, registry);
				}
			}
		}

	private:
		template <typename T>
		bool Read(T& out)
		{
			return ReadBytes((char*)&out, sizeof(T));
		}
		int64_t ReadBytes(char* buf, size_t size);

		template <typename T>
		bool Write(const T& val)
		{
			return WriteBytes(reinterpret_cast<char*>((void*)&val), sizeof(T));
		}
		bool WriteBytes(const char* buf, size_t n);

	private:
		std::fstream mStream;
		bool mRead;
	};
}