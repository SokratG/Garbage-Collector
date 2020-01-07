#pragma once
#include <string_view>
#include <exception>

using std::string_view;
using std::exception;


class OutOfRangeException : public exception
{
private:
	string_view Message;

public:
	OutOfRangeException() : Message{ "Error: out of range pointer " } { }

	string_view&& getMessage() noexcept
	{
		return std::move(Message);
	}
};

class FailExpectedSize : public exception
{
private:
	string_view Message;

public:
	FailExpectedSize() : Message{ "Error: fail size pass to function " } { }

	string_view&& getMessage() noexcept
	{
		return std::move(Message);
	}
};


class TimeOutException : public exception
{
private:
	string_view Message;

public:
	TimeOutException() : Message{ "Error: mutex timeout period exceeded " } { }

	string_view&& getMessage() noexcept
	{
		return std::move(Message);
	}
};
