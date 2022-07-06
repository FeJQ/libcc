#pragma once


#include<iostream>
#include<string.h>
#include <vector>
#include "ElemImpl.hpp"
#include <string>



namespace libcc
{
	namespace element
	{
		class ListImpl
		{
		public:
			ListImpl() = default;
			ListImpl(const ListImpl& list, bool isRestruct = true)
			{
				if (isRestruct)
				{
					for (int i = 0; i < list.data.size(); i++)
					{
						this->data.push_back(list.data[i]);
					}
					return;
				}
				this->data.push_back(list);
			}
			ListImpl(const std::string& str)
			{
				for (int i = 0; i < str.size(); i++)
				{
					this->data.push_back((unsigned char)str[i]);
				}
			}



			/* 其实现必须在头文件,否则会找不到符号 */
			template<typename... Arg>
			ListImpl(Arg... args)
			{
				append(args...);
			}

			template<typename T>
			void append(T value)
			{
				data.push_back(value);
			}


			template <typename Head, typename... Rail>
			void append(Head head, Rail... last)
			{
				data.push_back(head);
				append(last...);
			}


			ListImpl operator+(const int i)
			{
				this->data.push_back(i);
				return this;
			}
			ListImpl operator+(const ListImpl& list)
			{
				ListImpl tempList;
				for (int i = 0; i < this->data.size(); i++)
				{
					tempList.data.push_back(this->data[i]);
				}
				for (int i = 0; i < list.data.size(); i++)
				{
					tempList.data.push_back(list.data[i]);
				}
				return tempList;
			}

			ListImpl operator=(const ListImpl& list)
			{
				this->data.clear();
				for (int i = 0; i < list.data.size(); i++)
				{
					this->data.push_back(list.data[i]);
				}
				return this;
			}


			ListImpl operator+=(const std::string& str)
			{
				for (int i = 0; i < str.size(); i++)
				{
					this->data.push_back(str[i]);
				}
				return this;
			}
			ListImpl operator+=(const ListImpl& list)
			{
				for (int i = 0; i < list.data.size(); i++)
				{
					this->data.push_back(list.data[i]);
				}
				return this;
			}
			ListImpl operator+=(unsigned char c)
			{
				this->data.push_back(c);
				return this;
			}

			ElemImpl& operator[](int index)
			{
				return data[index];
			}

			void set(int index, ElemImpl a)
			{
				this->data[index] = a;
			}
			ElemImpl get(int index)
			{
				return this->data[index];
			}

			void foreach(void(*callback)(ElemImpl&))
			{
				for (int i = 0; i < this->data.size(); i++)
				{
					callback(this->data[i]);
				}
			}

			std::string toByteString()
			{
				std::string str;
				for (int i = 0; i < this->data.size(); i++)
				{
					str += this->data[i].toByte();
				}
				if (str.size() != this->data.size()) throw std::exception(("List to string failed," + std::to_string(this->data.size() - str.size()) + " byte(s) are missing").c_str());
				return str;
			}
			int size()
			{
				return this->data.size();
			}

			bool empty()
			{
				return this->data.size() == 0;
			}
			bool exist(int index)
			{
				if (this->data.size() >= index && !this->data[index].empty())
				{
					return true;
				}
				return false;
			}
			static void fromJson(std::string jsonValue) {

			}
			std::string toIntArray()
			{
				std::string intArray;
				for (auto i = 0; i < this->data.size() - 1; i++)
				{
					intArray += std::to_string((unsigned int)data[i].toByte()) + ", ";
				}
				intArray += std::to_string((unsigned int)data[this->data.size() - 1].toByte());
				return intArray;
			}
			std::string toStringArray()
			{
				std::string array = "[";
				for (auto i = 0; i < this->data.size(); i++)
				{
					array += data[i].toString() + (i == (this->data.size() - 1) ? "" : ",");
				}
				array += "]";
				return array;
			}

			std::vector<ElemImpl>::iterator begin()
			{
				return this->data.begin();
			}

			std::vector<ElemImpl>::iterator end()
			{
				return this->data.end();
			}

		private:
			std::vector<ElemImpl>  data;
		};


	}
}