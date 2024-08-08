#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <curl/easy.h>
#include <string>
#include <vector>
#include <locale>

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

std::fstream download(std::string episode);


int main(){
    int i = 0;
    std::fstream file = download("/works/1177354054894027232/episodes/16817330664850590717");
    while(1){
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
                std::cout << line;
                episode = line.substr(line.find_first_of("\"") + 1, line.find('"', line.find_first_of('"') + 1) - line.find_first_of("\"") - 1 );
                i = 1;
            }else{
                if(i == 0) episode = "asdasd";
            }
        }
        title = title.substr(title.find_first_of(">") + 1, 9);

        std::fstream output;
        std::cout << title;
        output.open("./outputs/" + title + ".txt", std::ios::out);

        while(lines.size() != 0){
            output << lines.front() << std::endl;
            lines.erase(lines.begin());
        }   
        if(episode.find("asdasd") != std::string::npos){
            break;
        }
        file.close();
        file = download(episode);
        i = 0;
    }
    return 0;
}

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata){
    std::fstream* file = static_cast<std::fstream*>(userdata);
    file->write(ptr, size*nmemb);
    return size * nmemb;
}

std::fstream download(std::string episode){
    std::string url = "https://kakuyomu.jp";

    std::fstream file;
    file.open("output.html", std::ios::binary | std::ios::out | std::ios::in);
    std::setlocale(LC_ALL, "ja_JP.UTF-8");
    //file.imbue(std::locale("ja_JP.UTF-8"));

    CURL* curl = curl_easy_init();

    char error[CURL_ERROR_SIZE];

    curl_global_init(CURL_GLOBAL_ALL);

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
    curl_global_cleanup();

    return file;

}
