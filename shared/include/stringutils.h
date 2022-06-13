#pragma once

static void trim(std::string& str)
{
        const auto first_not_space = std::find_if(str.begin(), str.end(),
                                                  [](const char ch)
                                                  {
                                                          return !std::isspace(ch);
                                                  });

        str.erase(str.begin(), first_not_space);

        const auto last_not_space = std::find_if(str.rbegin(), str.rend(),
                                                 [](const char ch)
                                                 {
                                                         return !std::isspace(ch);
                                                 })
                                        .base();

        str.erase(last_not_space, str.end());
}

static void split(std::vector<std::string>& vec, const std::string& str)
{
        auto start = 0;
        auto end = str.find(' ');

        while(end != str.npos)
        {
                vec.push_back(str.substr(start, end - start));
                start = end + 1;
                end = str.find(' ', start);
        }

        vec.push_back(str.substr(start));
}
