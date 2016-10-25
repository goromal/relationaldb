#!/bin/bash
pmccabe code/*.h code/*.cpp > _pmccabe_data
./__pparser_ub 6 _pmccabe_data
rm _pmccabe_data
