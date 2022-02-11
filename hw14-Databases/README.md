# Databases: connect to database and calculate some statistics for column in specified table

Program uses ```libpq``` library to connect to PostgreSQL database.

Program calculates the following statistics for any integer table in the specified table:
1. Minimum and maximum values;
2. Sum of all values;
3. Average value;
4. Variance(S);

# Input arguments
Program requires exactly 3 input arguments in the following order:

- database name;
- table name (must correspond to integer type);
- column name;

When the program starts, it requests username and password for the database access.
