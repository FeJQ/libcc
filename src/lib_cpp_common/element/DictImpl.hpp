#pragma once


#include<iostream>
#include<string.h>
#include <map>
#include "ElemImpl.hpp"
#include <string>


namespace libcc
{
	namespace element
	{
		class DictImpl
		{
		public:
			DictImpl() = default;
			DictImpl(const DictImpl& dict)
			{
				this->update(const_cast<DictImpl&>(dict));
			}
			DictImpl(std::pair<ElemImpl, ElemImpl>)
			{

			}

			ElemImpl& operator[](const ElemImpl& key)
			{
				std::string strKey;
				ElemImpl& e = const_cast<ElemImpl&>(key);
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
					this->data[strKey] = ElemImpl();
				}
				return this->data[strKey];
			}

			DictImpl& operator+(DictImpl& dict)
			{
				this->update(dict);
				return *this;
			}

			void operator=(const DictImpl& dict)
			{
				this->data.clear();
				this->update(dict);
			}

			void operator+=(DictImpl dict)
			{
				this->update(dict);
			}


			std::map<std::string, ElemImpl>::iterator begin()
			{
				return this->data.begin();
			}

			std::map<std::string, ElemImpl>::iterator end()
			{
				return this->data.end();
			}



			void foreach(void callback(std::string& key, ElemImpl& value))
			{
				for (auto i = this->data.begin(); i != this->data.end(); i++)
				{
					callback((std::string&)i->first, (ElemImpl&)i->second);
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

			void update(const DictImpl& dict)
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
			std::map<std::string, ElemImpl> data;
		};
	}
}