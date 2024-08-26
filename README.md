# Variation descriptions
* WFC.cpp - Normal wave function collapse, but the given tileset must be made, so that every cell can be filled with atleast one tile no matter what the neighbour is
* WFCwithBacktracking.cpp - Implemented backtracking to resolve conflicts while generating (Relatively slow solution)
* WFCwithBBM.cpp - Uses BBM (Block Based Method), where if a conflict is encountered, remove a chunk at that location and continue generating (Best solution so far)
* WFCwithReset.cpp - When conflict is encountered, reset the grid (Good solution but extremely slow on big grids)
