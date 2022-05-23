#pragma once


#include<iostream>
#include<string.h>
#include <map>
#include "Elem.hpp"


namespace libcc
{
	namespace element
	{
		class Dict
		{
		public:
			Dict() = default;
			Dict(const Dict& dict)
			{
				this->update(const_cast<Dict&>(dict));
			}
			Dict(std::pair<Elem, Elem>)
			{

			}

			Elem& operator[](const Elem& key)
			{
				std::string strKey;
				Elem& e = const_cast<Elem&>(key);
				std::string name = e.type().name();
				if (e.isType<std::string>())
				{
					strKey = e.cast<std::string>();
				}
				else if (e.isType<int>())
				{
					strKey = std::to_string(e.cast<int>());
				}
				else if (e.isType<char*>())
				{
					strKey = e.cast<char*>();
				}
				else if (e.isType<const char*>())
				{
					strKey = e.cast<const char*>();
				}
				else
				{
					throw std::exception(("unsupport type " + name).c_str());
				}
				if (!this->exist(strKey))
				{
					this->data[strKey] = Elem();
				}
				return this->data[strKey];
			}

			Dict& operator+(Dict& dict)
			{
				this->update(dict);
				return *this;
			}

			void operator=(const Dict& dict)
			{
				this->data.clear();
				this->update(dict);
			}

			void operator+=(Dict dict)
			{
				this->update(dict);
			}


			std::map<std::string, Elem>::iterator begin()
			{
				return this->data.begin();
			}

			std::map<std::string, Elem>::iterator end()
			{
				return this->data.end();
			}



			void foreach(void callback(std::string& key, Elem& value))
			{
				for (auto i = this->data.begin(); i != this->data.end(); i++)
				{
					callback((std::string&)i->first, (Elem&)i->second);
				}
			}

			int size()
			{
				return this->data.size();
			}

			bool empty()
			{
				return this->data.empty();
			}

			bool exist(int key)
			{
				return this->data.find(std::to_string(key)) != this->data.end();
			}
			bool exist(std::string key)
			{
				return this->data.find(key) != this->data.end();
			}

			void update(const Dict& dict)
			{
				int a = 0;
				for (auto i = dict.data.begin(); i != dict.data.end(); i++)
				{
					this->data[i->first] = i->second;
				}
			}

			std::vector<std::string> keys()
			{
				std::vector<std::string> keys;
				for (auto i = this->data.begin(); i != this->data.end(); i++)
				{
					keys.push_back(i->first);
				}
				return keys;
			}

			std::string toStringArray()
			{
				std::string array = "{";
				int count = 0;
				for (auto i = this->begin(); i != this->end(); i++)
				{
					array = array + i->first + ":" + i->second.toString() + (count == this->size() - 1 ? "" : ",");
					count++;
				}
				array += "}";
				return array;
			}


		private:
			std::map<std::string, Elem> data;
		};
	}
}