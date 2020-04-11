#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <jsoncpp/json/json.h>
struct contact_months
{
  std::string name;
  std::vector<std::string> months;
  std::vector<long int> char_count;
};

int main()
{
  // Vamos configurar os parâmetros com um arquivo JSON
  std::ifstream config("result.json");
  // Antes do parse escrevemos os conteúdos do arquivo em
  // uma string
  std::ostringstream sstr;
  char c;
  while (config.get(c))
  {
    sstr << c;
  }
  std::string readBuffer = sstr.str();
  Json::Reader reader;
  Json::Value chats;
  reader.parse(readBuffer, chats);
  std::vector<contact_months> contacts;
  for (int i = 0; i < chats["chats"]["list"].size(); i++)
  {
    const auto contact = chats["chats"]["list"][i];
    //std::cout << contact["name"].asString() << ": " << std::flush;
    const std::string name = contact["name"].asString();

    const contact_months con_mon {name};
    contacts.push_back(con_mon);


    std::string curr_month;
    for (int j = 0; j < contact["messages"].size(); j++)
    {
      const auto message = contact["messages"][j];
      if (message.isMember("text"))
      {
        if (message["text"].isString())
        {
          const auto message_month = message["date"].asString().substr(0, 4);
          const long int message_char_count = message["text"].asString().size();
          if (message_month == curr_month)
          {
            // Atualizando mês antigo
            contacts[i].char_count.back() += message_char_count;
          }
          else
          {
            // Novo mês
            curr_month = message_month;
            contacts[i].months.push_back(curr_month);
            contacts[i].char_count.push_back(message_char_count);
          }
        }
      }
    }
  }
  std::ofstream logfile("bar_race.csv", std::ios::out | std::ios::trunc | std::ios::ate);
  if (!logfile.is_open())
  {
    std::cerr << "Não foi possível criar o arquivo" << std::endl;
    return -1;
  }
  logfile << "Nome,,";
  for (int i = 2015; i < 2021; i++)
  {
    logfile << "," << i;
  }
  logfile << std::endl;
  for (const auto &con : contacts)
  {
    logfile << con.name << ",,";
    int j = 0;
    for (int i = 2015; i < 2021; i++)
    {
      if (j < con.months.size())
      {
        if (std::stoi(con.months[j]) == i)
        {
          logfile << "," << con.char_count[j];
          j++;
        }
        else
        {
          logfile << ",0";
        }
      }
      else
      {
        logfile << ",0";
      }
    }
    logfile << std::endl;
  }
  logfile.close();
}
