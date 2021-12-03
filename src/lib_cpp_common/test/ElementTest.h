#pragma once
#include "test/BaseTest.h"
#include "element/List.hpp"
#include "element/Dict.hpp"


namespace libcc
{
	namespace test
	{
		using element::List;
		using element::Dict;
		using std::cout;
		using std::endl;
		using std::string;
		class ElementTest :public BaseTest
		{
		public:
			virtual bool entry() 
			{
				bool result=true;
				result &= listTest();
				result &=  dictTest();
				return result;
			}
		private:
			bool listTest()
			{
				char c[] = { 'a','b','c','\0' };
				List list1(1, 'a', "hello world", true, (short)1234, (long long)56798, 1.2f, 3.14159264);
				List list2(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
				List list3(11, 12, 13, 14, 15, 16, 17, 18, 19, 20);

				// 往list1结尾追加元素(或 list1+=c;)
				list1.append(c);
				cout << list1.toStringArray() << endl;

				// 遍历list2
				for (auto i = 0; i < list2.size(); i++)
				{
					cout << list2[i].toString();
				}
				cout << endl;

				// 拼接list2和list3
				List listA = list2 + list3;
				cout << listA.toStringArray() << endl;

				// 当List中数据只有C语言基本类型时,List和string可以互相转换
				string byteStr = listA.toByteString();
				List strList = List(byteStr);
				cout << strList.toStringArray() << endl;

				// 默认情况下,构造List时传入List类型,会将ListA中的元素转移过去,如果第二个参数为falst,则会产生List嵌套
				List(listA, true);
				return true;
			}


			bool dictTest()
			{
				Dict dict1;
				dict1["name"] = "FeJQ";
				dict1["height"] = 180;
				dict1["male"] = true;

				if (!dict1.exist("email"))
				{
					dict1["email"] = "a@b.com";
				}

				for (auto i = dict1.begin(); i != dict1.end(); i++)
				{
					cout << i->first << ":" << i->second.toString() << endl;
				}
				cout << dict1.toStringArray() << endl;
				return true;

			}


		};

	}
}