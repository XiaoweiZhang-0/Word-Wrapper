1. (0 points) Create a C source file and copy the following function definition into it:

<pre>void triangle(unsigned width)
{
    unsigned i, j;
    
    i = 0;
    while (i <= width) {
        j = 0;
        while (j < i) {
            putchar('*');
            ++j;
        }
        putchar('\n');
        ++i;
    }
}</pre>

Add a main function that calls triangle() with a number of your choosing, and complete the source file such that you can compile and execute your program.

2. (3 points) Rewrite triangle() to use for loops instead of while loops.
3. (3 points) Write a version of triangle() called v_triangle() that prints the triangle upside-down, with the maximum length line at the top.
4. (3 points) Write a version of triangle() called h_triangle() that prints the triangle such that the right side is vertical. That is, h_triangle(5) should print

<pre>*****
 ****
  ***
   **
    *
</pre>

5. (3 points) Create a program that can print any of the three triangles described above, based on its command-line arguments. Use atoi() to convert the first argument from a string to an integer: this will be the width of the triangle. If no second argument is provided, use triangle(). If the second argument is "v", use v_triangle(). If it is "h", use h_triangle().

Your program should return EXIT_SUCCESS from main for normal operation, or EXIT_FAILURE if given incorrect/inappropriate arguments.
