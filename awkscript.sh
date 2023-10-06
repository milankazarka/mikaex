#!/bin/bash
awk '
 {
   for (i=1;i<=NF;i++) {
        if ( $i ~ /[[:alpha:]]@[[:alpha:]]/ )  { 
       print $i      
        }
   }
}' "pages/page2.html"
