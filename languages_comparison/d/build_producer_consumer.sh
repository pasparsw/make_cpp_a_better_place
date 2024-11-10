#!/bin/bash

dmd -O -release -inline -boundscheck=off producer_consumer.d
