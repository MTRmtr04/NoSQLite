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
    public:
        collection(const std::string &name);
        ~collection();
    };
    
} // namespace nosqlite




#endif