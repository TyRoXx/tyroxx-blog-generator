compile(
    "#How to choose an integer type\n"
    "(created 2017-04-05, updated 2017-04-05)\n"
    "Prefer portable types like `std::uint32_t` over the types without an "
    "explicit range. Use `int`, `long`, `long long`"
    "only if you have a reason to use exactly these and not the portable ones. "
    "C++ is different from Java and C#: `int` etc are not the same size and "
    "range on every platform. This makes "
    "them hard to use even for C++ experts. They are neither necessary nor "
    "sufficient for most practical situations.\n"
    "In most cases you already know the expected range of values. So why not "
    "use the right integer type for this instead instead of "
    "cargo-culting it?\n"
    "If you know that you are dealing with 64-bit file sizes, document "
    "this assumption in an unambiguous way. There is no reason for "
    "using `int` or `long long` in this case. Using the wrong type leads to "
    "unportable code and "
    "misunderstandings.") +
    snippet_from_file(
        snippets_source_code,
        ventura::relative_path("how-to-choose-an-integer-type-0.cpp")) +
    compile("One of the few valid use cases for the built-in types is "
            "overloading a function for all integer types that exist:") +
    snippet_from_file(
        snippets_source_code,
        ventura::relative_path("how-to-choose-an-integer-type-1.cpp")) +
    compile("This kind of code should be fairly uncommon though.\n The other "
            "major kind of integer types are the pointery ones: `std::size_t`, "
            "`std::ptrdiff_t` and `std::uintptr_t`. They are used when dealing "
            "with memory on the current machine. "
            "`int` cannot store the sizes of objects on most contemporary "
            "machines. `size_t` can. Use `size_t`. `ptrdiff_t` is for"
            " differences between memory addresses. It is signed because a "
            "difference can be negative. `uintptr_t` is for manipulating "
            "pointers on the bit level. If you did not know about `uintptr_t` "
            "before reading this article, chances are you won't need this type "
            "any soon. "
            "`size_t` and `ptrdiff_t` are two of the most important types in "
            "C++ you have to know and make use of."
            " This is a very small function how it would be written by someone "
            "who does not waste a single thought on choosing the right type.") +
    snippet_from_file(
        snippets_source_code,
        ventura::relative_path("how-to-choose-an-integer-type-2.cpp")) +
    compile("These are my thoughts when I read this function. I tried to mark "
            "the WTF moments so that you can calculate the WTFs per line "
            "metric as a take-home exercise if you like.") +
    snippet_from_file(
        snippets_source_code,
        ventura::relative_path("how-to-choose-an-integer-type-3.cpp")) +
    compile("I only have to change one little thing to make all these WTFs go "
            "away:") +
    snippet_from_file(
        snippets_source_code,
        ventura::relative_path("how-to-choose-an-integer-type-4.cpp")) +
    compile("Much better, isn't it? Is it really too much to ask for using the "
            "right integer types?")