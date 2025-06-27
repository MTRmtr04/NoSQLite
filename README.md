# NoSQLite

NoSQLite is a lightweight, local, and schema-less NoSQL database system designed to store and manage unstructured data using JSON files.
It offers a simple and flexible alternative to traditional relational databases and heavier NoSQL engines, requiring no server or external dependencies.

## Purpose

The main goal of NoSQLite is to provide an intuitive, easy-to-use document database system for:
    - Small-scale or personal applications
    - Educational tools and academic projects
    - Prototyping systems that work with unstructured or variable data

By mimicking core NoSQL concepts such as flexible schemas and document-based storage, NoSQLite enables developers to quickly build and test projects without worrying about strict tables and structures or complex setup processes.

## Features

    - JSON-based storage
    - CRUD operations (Create, Read, Update, Delete)
    - Unique file paths generated from document hashes
    - Collections that allow heterogeneous document structures


## How to use it

NoSQLite uses an external library for the manipulation of JSON files and objects. Its' documentation can be found in the following Github repository:
[nlohman JSON Library](https://github.com/nlohmann/json).

**WARNING**: you need `C++17` or greater to use NoSQLite.

### Instalation

Download the most recent release from the repository. Extract the file into a directory of your choosing.

Enter the extracted directory (with `cd`) and run the following commands to compile the showcasing code:

For Linux and MacOs:
```
g++ main.cpp -o nosqlite -I include/ lib/libnosqlite.a -fopenmp
```

For Windows:
```
cl main.cpp /EHsc /I include /std:c++17 /openmp /link /OUT:nosqlite.exe /LIBPATH:lib libnosqlite.lib
```

Read the comments inside the `main.cpp` file for help on how to run the program.


### Starter Knowledge

Before using NoSQLite there are a few things that should be kept in mind.
There are three main data types used:
 - `json` - The documentation for this one can be found in the link above.
 - `field_type` - This one is simply the `typedef` of a `std::vector<std::string>` and it is used to represent a field in a JSON document. JSON document can have an arbitrary number of nested fields, this data type is used to represent that inherent recursivity. The field `{"imdb", "rating"}` represents the `rating` field inside the `imdb` field.
 - `condition_type` - This data type is used to represent a condition on a query (this will be explained later). It is a struct with the following members: a `field_type`, an operation (can be one of the following: "==", "!=", "<", ">", "<=", ">=") and a `json` object with the target value. The condition `{{"imdb", "rating"}, ">=", 8.7}` represents any document where the imdb.rating field is greater or equal to 8.7.

### Set up

The first step is to set up the database itself. If the database hasn't been built yet it can be done with a single line of code (it might take some time to run if there are a lot of JSON documents):

```
nosqlite_api api("path/to/new/database", "path/to/JSON/files");
```

The "path/to/new/database" directory should be an empty directory and "path/to/JSON/files" should be a directory with multiple subdirectories. Each of these subdirectories will be taken as a separate collection of the new database.


If the database has already been build it can also be set up with a single line of code:

```
nosqlite_api api("path/to/database");
```

The "path/to/database" directory should be contain the database and the database alone.

### Querying

> NoSQLite supports all four basic CRUD operations as well as some more complicated queries constrained by conjunctions of simple conditions (e.g.: "year" < 2024 && "rating" >= 7.3)

There are six essential methods in the NoSQLite API for querying the database, they are the following:
 - `create` - Sets up the creation of a new document in the database.

 - `read` - Sets up a read operation on the database.
 
 - `update` - Sets up an update operation on the database.
 
 - `delete` - Sets up a delete operation on the database.
 
 - `AND` - Adds a new conditions to the operations above. Has no effect on the `create` operation.
 
 - `execute` - Executes the first four operations and return the result by reference.

#### Create

Creating a document in NoSQLite is extremely simple (as is every other operation).

```
vector<json> result = {};

api.create("movies", {
    {"title", "Memories of Murder"},
    {"director", "Bong Joon Ho"},
    {"year", 2003},
    {"imdb", {{"rating", 8.1}}}
})->execute(result);

// OR

api.create("movies", {
    {"title", "Memories of Murder"},
    {"director", "Bong Joon Ho"},
    {"year", 2003},
    {"imdb", {{"rating", 8.1}}}
})
api.execute(result);

```
They both to exactly the same thing and the `result` vector is always emptied (`result = {}`). This code adds the JSON document described by the second parameter of the `create` method call to the collection with name the same as the first parameter of the call. For more information on the JSON object refer to the JSON library link above.


#### Read

The simplest read of all, simply gets all of the documents in a certain collection.

```
api.read("movies");
api.execute(result);

// Just like the create operation the rad can be done both ways.
```

This code will fetch all of the documents in the collection "movies" and put them em the `result` vector.


The following code fetches a single document given its' id. All other conditions are ignored.

```
api.read("movies", {{"id"} "==", 123456789})->execute(result);

api.read("movies")->AND({{"id"} "==", 123456789})->execute(result);

api.read("movies", {{"year"} ">=", 2011})->AND({{"id"} "==", 123456789})->execute(result);
```

All three lines of code do the exact same thing. They read the document whose `id` is `123456789`. All of the other conditions (e.g.: `{{"year"} ">=", 2011}`) are ignored.

For complex queries (conjunction of conditions) the following code is a good example:

```
// All blocks perform the exact same query.

api.read("movies", {
    {"year"}, ">=", 2016
})->AND({
    {"imdb", "rating"}, "==", 8.0
})->AND({
    {"genres"}, "==", {"Documentary"}
})->execute(result);


api.read("movies");
api.AND({{"imdb", "rating"}, "==", 8.0})->AND({{"genres"}, "==", {"Documentary"}});
api.AND({{"year"}, ">=", 2016});
api.execute(result);


api.read("movies", {{"year"}, ">=", 2016});
api.AND({{"genres"}, "==", {"Documentary"}});
api.AND({{"imdb", "rating"}, "==", 8.0});
api.execute(result);

```

The order in which the conditions are added does not change the result, i.e. any order is valid.

#### Update

The update operation is similar to the read, there is only one small difference, the `update` method call has one more parameter.

```
api.update("movies", {
    {"imdb", {{"rating", 5.3}}}
})


api.update("movies", {
    {"imdb", {{"rating", 5.3}}}
}, {{"year"}, ">", 2017});
```

The first line of code updates every documents in the collection, setting the imdb rating field to 5.3. The second does the same but only on the documents where the year field is greater than 2017.

Adding more conditions to the update query is identical to adding conditions to the read operation.

Updates can also be done by document `id` like the read operation.

The `result` vector is populated with the updated documents.

#### Remove

The operation to remove documents from the database is identical to the read operation, the only change is the method called, instead of calling `read`, `remove` should be called.

The main difference is that if no conditions are provided the removal query will fail.

Once again the `result` vector is emptied.

#### Other Ops

There are four other possible operations on the database. Two of them operate on a collection as a whole, and the other two deal with the hash indexes on each collection.

In all four of these operations the `result` vector is emptied.

##### On Collections
The first one is the collection creation, and there are two variants:
```
// This one creates an empty collection
api.create_collection("books")->execute(result);

// This one creates a collection and fills it with the JSON documents in the specified path
api.create_collection("books", "path/to/books")->execute(result);
```

The second operation is the deletion of an entire collection.
```
api.delete_collection("books")->execute(result);
```

##### On Indexes
The first is the creation of a hash index on a certain collection, on a certain field. Beware that this operation can be costly if the collection is very large, however every equality query henceforth will be significantly faster (e.g.: `{{"imdb", "rating"}, "==", 8.0}`)
```
api.create_index("movies", {"imdb", "rating"});
```

The second is simply the deletion of a hash index:
```
api.delete_index("movies", {"imdb", "rating"});
```

## Devs
- Mateus Lima
- Max Ribeiro
- Tiago Oliveira
- Sara Azevedo