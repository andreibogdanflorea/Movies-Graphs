<div align="left">
  <h1>Movies Graphs</h1>
  <h3>A C project that uses Graphs to manipulate a database of Movies and Actors</h3>
</div>
<br/>
<br/>

## General Description ##
A "Production" graph is created, in which each vertex represents an actor and a link represents
the fact that the two connected actors were part of a common movie.

## Tasks ##
The first task computes the maximum number of actors connected (the maximum number of vertices in 
any connected component).

The second task computes how close two given actors are (the minimum distance between two vertices).

The third task determines the actors (vertices) that form a [bridge](https://en.wikipedia.org/wiki/Bridge_(graph_theory)).

## Files formats ##
The input file contains the number of movies on the first row. The following rows contain information about
those movies.

For each movie, the first row represents the Film Title, followed on the next row by the number of actors
in the cast, followed by the names of those actors, one on each row.

## How to run ##
To run, use:

`./movies -c<number> <input file> <output file>`

where <number> can be 1, 2 or 3, each representing the task 1, 2 or 3 previously described.
