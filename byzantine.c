{\rtf1\ansi\ansicpg1252\cocoartf2709
\cocoatextscaling0\cocoaplatform0{\fonttbl\f0\fswiss\fcharset0 Helvetica;}
{\colortbl;\red255\green255\blue255;}
{\*\expandedcolortbl;;}
\paperw11900\paperh16840\margl1440\margr1440\vieww11520\viewh8400\viewkind0
\pard\tx720\tx1440\tx2160\tx2880\tx3600\tx4320\tx5040\tx5760\tx6480\tx7200\tx7920\tx8640\pardirnatural\partightenfactor0

\f0\fs24 \cf0 #include <mpi.h>\
#include <stdio.h>\
#include <stdlib.h>\
#include <time.h>\
\
#define MAX_GENERALS 100\
#define MAX_TRAITORS 33\
\
int main(int argc, char** argv) \{\
    int rank, size, i, j;\
    int army_strength[MAX_GENERALS];\
    int received_strength[MAX_GENERALS];\
    int final_decision;\
    int traitor_count = 0;\
    int is_traitor = 0;\
\
    MPI_Init(&argc, &argv);\
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);\
    MPI_Comm_size(MPI_COMM_WORLD, &size);\
\
    if (size < 4) \{\
        if (rank == 0) \{\
            printf("This program requires at least 4 processes.\\n");\
        \}\
        MPI_Finalize();\
        return 1;\
    \}\
\
    srand(time(NULL) + rank);\
    \
    // Initialize army strength\
    army_strength[rank] = rand() % 100 + 1;\
    \
    // Randomly assign traitors\
    if (rank == 0) \{\
        traitor_count = size / 3;\
        int traitors[MAX_TRAITORS];\
        for (i = 0; i < traitor_count; i++) \{\
            traitors[i] = rand() % size;\
        \}\
        MPI_Bcast(traitors, traitor_count, MPI_INT, 0, MPI_COMM_WORLD);\
    \} else \{\
        int traitors[MAX_TRAITORS];\
        MPI_Bcast(traitors, size / 3, MPI_INT, 0, MPI_COMM_WORLD);\
        for (i = 0; i < traitor_count; i++) \{\
            if (traitors[i] == rank) \{\
                is_traitor = 1;\
                break;\
            \}\
        \}\
    \}\
\
    // Exchange army strengths\
    MPI_Allgather(&army_strength[rank], 1, MPI_INT, received_strength, 1, MPI_INT, MPI_COMM_WORLD);\
\
    // Traitors may lie about their army strength\
    if (is_traitor) \{\
        for (i = 0; i < size; i++) \{\
            if (i != rank) \{\
                int fake_strength = rand() % 100 + 1;\
                MPI_Send(&fake_strength, 1, MPI_INT, i, 0, MPI_COMM_WORLD);\
            \}\
        \}\
    \} else \{\
        for (i = 0; i < size; i++) \{\
            if (i != rank) \{\
                MPI_Send(&army_strength[rank], 1, MPI_INT, i, 0, MPI_COMM_WORLD);\
            \}\
        \}\
    \}\
\
    // Receive and validate strengths\
    for (i = 0; i < size; i++) \{\
        if (i != rank) \{\
            int received;\
            MPI_Recv(&received, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);\
            if (received != received_strength[i]) \{\
                printf("General %d detected inconsistency from General %d\\n", rank, i);\
            \}\
        \}\
    \}\
\
    // Make decision based on majority\
    int total_strength = 0;\
    for (i = 0; i < size; i++) \{\
        total_strength += received_strength[i];\
    \}\
    final_decision = (total_strength > size * 50) ? 1 : 0;\
\
    // Print results\
    printf("General %d %s: Army strength = %d, Final decision: %s\\n", \
           rank, \
           is_traitor ? "(Traitor)" : "(Loyal)", \
           army_strength[rank], \
           final_decision ? "ATTACK" : "RETREAT");\
\
    MPI_Finalize();\
    return 0;\
\}\
}