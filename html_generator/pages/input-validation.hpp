h3(link("#", "input-validation", "Input validation")) + h4(text("Integers")) +
    text("................") +
    snippet_from_file(
        snippets_source_code,
        ventura::relative_path("input-validation-integers_0.cpp")) +

    h4(text("Pointers")) + text("If you want to pass pointers that can't be ") +
    inline_code("nullptr") + text(" make them references.") +
    snippet_from_file(
        snippets_source_code,
        ventura::relative_path("input-validation-pointers_0.cpp")) +
    text("text after code")