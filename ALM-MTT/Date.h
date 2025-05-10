/*
    MIT License

    Copyright (c) 2025 Harold James Krause

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once

#include <tuple>
#include <string>
#include <algorithm>

namespace ALM {

    struct YearMonthDay {
        int year;
        int month;
        int day;
    };

    enum class Weekday {
        Sunday = 0,
        Monday,
        Tuesday,
        Wednesday,
        Thursday,
        Friday,
        Saturday
    };

    struct Duration {
        enum class Unit {
            Days,
            Months,
            Years
        };
        int amount;
        Unit unit;

        Duration operator-() const {
            return Duration(-amount, unit);
        }
    };

    class Date {
        using SerialType = int32_t;

    public:
        Date() : serial_(-1) {}
        Date(SerialType serial) : serial_(serial) {}
        Date(YearMonthDay ymd) : serial_(YMDToSerial(ymd)) {}

        Weekday weekday() const {
            // 1970-01-01 was thursday...
            return static_cast<Weekday>((serial_ + 4) % 7);
        }
        int year() const { return serialToYMD(serial_).year; }
        int month() const { return serialToYMD(serial_).month; }
        int day() const { return serialToYMD(serial_).day; }
        SerialType serial() const { return serial_; }
        YearMonthDay toYMD() const { return serialToYMD(serial_); }

        std::string toStr() const {
            YearMonthDay ymd = serialToYMD(serial_);
            return std::to_string(ymd.month) 
                + "-" + std::to_string(ymd.day) 
                + "-" + std::to_string(ymd.year);
        }

        // Converts a year/month/day to a serial number
        static SerialType YMDToSerial(YearMonthDay ymd) {
            int year = ymd.year;
            int month = ymd.month;
            int day = ymd.day;
            if (month <= 2) {
                year -= 1;
                month += 12;
            }
            int era = (year >= 0 ? year : year - 399) / 400;
            int yoe = year - era * 400;
            int doy = (153 * (month - 3) + 2) / 5 + day - 1;
            int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
            int days = era * 146097 + doe - 719468; // 1970-01-01
            return static_cast<SerialType>(days);
        }

        // Converts a serial number to a year/month/day
        static YearMonthDay serialToYMD(SerialType serial) {
            int z = serial + 719468; // 1970-01-01
            int era = (z >= 0 ? z : z - 146096) / 146097;
            int doe = z - era * 146097;
            int yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
            int y = yoe + era * 400;
            int doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
            int mp = (5 * doy + 2) / 153;
            int d = doy - (153 * mp + 2) / 5 + 1;
            int m = mp < 10 ? mp + 3 : mp - 9;
            y += (m <= 2);
            return { y, m, d };
        }

        static int daysInMonth(int year, int month) {
            static const int mths[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30 ,31 };
            if (month != 2) return mths[month - 1];
            return isLeapYear(year) ? 29 : 28;
        }

        static bool isLeapYear(int y) {
            return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
        }

        bool operator==(const Date& rhs) const { return serial_ == rhs.serial_; }
        bool operator!=(const Date& rhs) const { return serial_ != rhs.serial_; }
        bool operator< (const Date& rhs) const { return serial_ < rhs.serial_; }
        bool operator<=(const Date& rhs) const { return serial_ <= rhs.serial_; }
        bool operator> (const Date& rhs) const { return serial_ > rhs.serial_; }
        bool operator>=(const Date& rhs) const { return serial_ >= rhs.serial_; }
        int operator-(const Date& rhs) const { return serial_ - rhs.serial_; }

        Date& operator+=(const Duration& dur) {
            switch (dur.unit) {
            case Duration::Unit::Days:
                *this = this->addDays(dur.amount);
                break;
            case Duration::Unit::Months:
                *this = this->addMonths(dur.amount);
                break;
            case Duration::Unit::Years:
                *this = this->addYears(dur.amount);
                break;
            }
            return *this;
        }

        Date operator-=(const Duration& dur) {
            return *this += (-dur);
        }

        Date operator+(const Duration& dur) const {
            Date result = *this;
            return result += dur;
        }

        Date operator-(const Duration& dur) const {
            return *this + (-dur);
        }



    private:
        SerialType serial_;

        Date addDays(int n) const {
            return Date(serial_ + n);
        }
        Date addMonths(int n) const {
            YearMonthDay ymd = serialToYMD(serial_);
            int y = ymd.year;
            int m = ymd.month;
            int d = ymd.day;

            int tot_m = m - 1 + n;
            int new_year = y + tot_m / 12;
            int new_month = tot_m % 12 + 1;
            if (new_month <= 0) {
                new_month += 12;
                new_year -= 1;
            }
            int max_day = daysInMonth(new_year, new_month);
            int new_day = std::min(d, max_day);
            return Date({ new_year, new_month, new_day });
        }
        Date addYears(int n) const {
            YearMonthDay ymd = serialToYMD(serial_);
            int y = ymd.year;
            int m = ymd.month;
            int d = ymd.day;
            int new_year = y + n;
            int max_day = daysInMonth(new_year, m);
            int new_day = std::min(d, max_day);
            return Date({ new_year, m, new_day });
        }
    };

}