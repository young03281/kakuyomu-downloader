#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <curl/easy.h>
#include <string>
#include <vector>
#include <locale>
#include <sstream>
#include <filesystem>

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

void download(std::string episode);


int main(){
    std::filesystem::create_directory("outputs");
    int i = 0;
    std::fstream file;
    download("/works/1177354054894027232/episodes/1177354054894027298");
    while(1){
        file.open("output.html", std::ios::in);
        file.seekg(0);
        std::string line, title, episode = "";
        std::vector<std::string> lines;
        while(!file.eof()){
            std::getline(file, line);
            if(line.find("<p class=\"widget-episodeTitle js-vertical-composition-item\">") != std::string::npos){
                title = line.data();
            }
            if(line.find("<p id=\"p") != std::string::npos){
                line = line.substr(line.find_first_of(">") + 1, line.find("<", 2) - line.find_first_of(">") - 1);
                if(line.find("<") != std::string::npos){
                    line = line.substr(line.find_first_of(">") + 1, line.find("<", 2) - line.find_first_of(">") - 1);
                }
                if(line.find("<br") != std::string::npos) line = "\n";
                lines.push_back(line);
            }
            if(line.find("id=\"contentMain-readNextEpisode\"") != std::string::npos && i == 0){
                episode = line.substr(line.find_first_of("\"") + 1, line.find('"', line.find_first_of('"') + 1) - line.find_first_of("\"") - 1 );
                i = 1;
            }else{
                if(i == 0) episode = "asdasd";
            }
        }
        std::stringstream ss;
        ss << title;
        char temp;
        int n = 0;
        while(!ss.eof()){
            ss >> temp;
            std::string digits = "0123456789";
            if(digits.find(temp) != std::string::npos){
                n++;
            }
        }
        title = title.substr(title.find('>') + 1, 6+n);
        std::fstream output;
        std::cout << title << '\n';
        output.open("./outputs/" + title + ".txt", std::ios::out);

        while(lines.size() != 0){
            output << lines.front() << std::endl;
            lines.erase(lines.begin());
        }   
        if(episode.find("asdasd") != std::string::npos){
            break;
        }
        file.close();
        download(episode);
        i = 0;
    }
    return 0;
}

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata){
    std::fstream* file = static_cast<std::fstream*>(userdata);
    file->write(ptr, size*nmemb);
    return size * nmemb;
}

void download(std::string episode){
    std::string url = "https://kakuyomu.jp";

    std::fstream file;
    file.open("output.html", std::ios::out);
    std::setlocale(LC_ALL, "ja_JP.UTF-8");
    //file.imbue(std::locale("ja_JP.UTF-8"));

    CURL* curl = curl_easy_init();

    char error[CURL_ERROR_SIZE];

    curl_easy_setopt(curl, CURLOPT_URL, (url + episode).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, "/");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; WOW64) Chrome/51.0.2704.103");

    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    file.close();
}
