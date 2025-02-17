/**
 * @file
 */
#ifndef COLLECTION_CLASS_H
#define COLLECTION_CLASS_H

#include <string>

namespace nosqlite {

    class collection {
    private:
        std::string name;
        int number_of_documents;

        void build_from_scratch(const std::string &path_to_json);
    public:
        collection(const std::string &name);
        collection(const std::string &name, const std::string &path_to_json);
        ~collection();
    };
    
} // namespace nosqlite




#endif