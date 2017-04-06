tags::h3(tags::link("#", "how-to-choose-an-integer-type",
                    "How to choose an integer type")) +
    tags::p("(created 2017-04-05, updated 2017-04-05)") +

    Si::html::tag("p", text("Prefer portable types like ") + inline_code("std::uint32_t") +
    text(" over the types without an explicit range. Use ") +
    inline_code("int") + text(", ") + inline_code("long") + text(", ") +
    inline_code("long long") +
    text(" only if you have a reason to use exactly these and not the portable "
         "ones. C++ is different from Java and C#: ") +
    inline_code("int") +
    text(" etc are not the same size and range on every platform. This makes "
         "them hard to use even for C++ experts. They are neither necessary "
         "nor sufficient for most practical situations.")) +

    tags::p(
        "In most cases you already know the expected range of values. So why "
        "not use the right integer type for this instead instead of "
        "cargo-culting it?") +
    Si::html::tag("p", text("If you know that you are dealing with 64-bit file sizes, document "
         "this assumption in an unambiguous way. There is no reason for "
         "using ") +
    inline_code("int") + text(" or ") + inline_code("long long") +
    text(" in this case. Using the wrong type leads to unportable code and "
         "misunderstandings.")) +
    snippet_from_file(
        snippets_source_code,
        ventura::relative_path("how-to-choose-an-integer-type-0.cpp")) +

    tags::p("One of the few valid use cases for the built-in types is overloading "
         "a function for all integer types that exist:") +
    snippet_from_file(
        snippets_source_code,
        ventura::relative_path("how-to-choose-an-integer-type-1.cpp")) +
    tags::p("This kind of code should be fairly uncommon though.") +

    Si::html::tag("p", text("The other major kind of integer types are the pointery ones: ") +
    inline_code("std::size_t") + text(", ") + inline_code("std::ptrdiff_t") +
    text(" and ") + inline_code("std::uintptr_t") +
    text(". They are used when dealing with memory on the current machine. ") +
    inline_code("int") +
    text(" cannot store the sizes of objects on most contemporary machines. ") +
    inline_code("size_t") + text(" can. Use ") + inline_code("size_t") +
    text(". ") + inline_code("ptrdiff_t") +
    text(" is for differences between memory addresses. It is signed because a "
         "difference can be negative. ") +
    inline_code("uintptr_t") + text(" is for manipulating pointers on the bit "
                                    "level. If you did not know about ") +
    inline_code("uintptr_t") + text(" before reading this article, chances are "
                                    "you won't need this type any soon. ") +
    inline_code("size_t") + text(" and ") + inline_code("ptrdiff_t") +
    text(" are two of the most important types in C++ you have to know and "
         "make use of.") +
    text(
        " This is a very small function how it would be written by someone who "
        "does not waste a single thought on choosing the right type.")) +

    snippet_from_file(
        snippets_source_code,
        ventura::relative_path("how-to-choose-an-integer-type-2.cpp")) +

    tags::p("These are my thoughts when I read this function. I tried to mark the "
         "WTF moments so that you can calculate the WTFs per line metric as a "
         "take-home excercise if you like.") +

    snippet_from_file(
        snippets_source_code,
        ventura::relative_path("how-to-choose-an-integer-type-3.cpp")) +

    tags::p("I only have to change one little thing to make all these WTFs go "
         "away:") +
    snippet_from_file(
        snippets_source_code,
        ventura::relative_path("how-to-choose-an-integer-type-4.cpp")) +
    tags::p("Much better, isn't it? Is it really too much to ask for using the "
            "right integer types?")
