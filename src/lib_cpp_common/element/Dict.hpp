#pragma once


#include<iostream>
#include<string.h>
#include <map>
#include "Elem.hpp"


namespace libcc
{
	namespace element
	{
		using std::string;
		class Dict
		{
		public:
			Dict() = default;
			Dict(const Dict& dict)
			{
				this->update(const_cast<Dict&>(dict));
			}
			Dict(pair<Elem, Elem>)
			{

			}

			Elem& operator[](const Elem& key)
			{
				string strKey;
				Elem& e = const_cast<Elem&>(key);
				string name = e.type().name();
				if (e.isType<string>())
				{
					strKey = e.cast<string>();
				}
				else if (e.isType<int>())
				{
					strKey = to_string(e.cast<int>());
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
					throw exception(("unsupport type " + name).c_str());
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


			map<string, Elem>::iterator begin()
			{
				return this->data.begin();
			}

			map<string, Elem>::iterator end()
			{
				return this->data.end();
			}



			void foreach(void callback(string& key, Elem& value))
			{
				for (auto i = this->data.begin(); i != this->data.end(); i++)
				{
					callback((string&)i->first, (Elem&)i->second);
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
				return this->data.find(to_string(key)) != this->data.end();
			}
			bool exist(string key)
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

			vector<string> keys()
			{
				vector<string> keys;
				for (auto i = this->data.begin(); i != this->data.end(); i++)
				{
					keys.push_back(i->first);
				}
				return keys;
			}

			string toStringArray()
			{
				string array = "{";
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
			map<string, Elem> data;
		};
	}
}