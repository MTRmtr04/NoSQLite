
# **NoSQLite structure (basic overview)**


# File structure
> This section describes how the database itself will be structured, how the JSON files will be organised.

## Database
The database itself will be a single folder. Inside this folder there will be two distict types of items, one JSON file with information about the database as a whole and one folder for each collection in the database.

## Collection
Each collection is a single folder as described above. Inside this folder there will be three distict types of items, one JSON file with information about the collection, one folder for the indices of the collection and multiple folders for the data.

The folders will be named and organized based on the document's ID number.  We'll use a hash based on this ID. The first two characters of the hash will determine the main folder's name. Inside each main folder, there will be more subfolders.  The names of these subfolders will be based on the next two characters of the hash. This method helps us distribute the documents evenly across all the folders.

## Index
The indices for each collection will be places inside the folder mentioned above. Each index will have its own subfolder named after the type of index and the field it indexes.

> More on indices here in the future


# Code structure
> This section describes how the code for NoSQLite will be structure, a high level overview of the classes needed and their functions. Any names given here are temporary and subject to change.

## User available
> The following classes will be available to the user.

### Database class (db)
This will be the main class the users interact with. It will hold the information about the database as in the file described above. An instance of this class will also hold an instance of the Collection class (described below) for each collection in the database.

### Collection class (collection)
This class will hold information about an individual collection and in the file described above. An instance of this class will also hold an instance of the Index class (described below) for each index in the collection. It will also allow users to create, read, update and delete JSON documents in this collection.

### Index class (index)
This class and any of its children (one for each type of index implemented) will be responsible for creating, updating and deleting any index. It will also be responsible for finding the correct files to be access according to the users input.

## User hidden
> The following classes will be hidden from the user (ideally).

### Database builder (db_builder)
This class will be responsible for the initial parsing of the JSON files to build the database itself as well as any editing required afterward.

### Extra
> There may be a need for more classes here.
