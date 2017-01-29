
h3(link("#", "input-validation", "Input validation")) + h4(text("Integers")) +
    text("................") +
    snippet_from_file(snippets_source_code, "input-validation-integers_0.cpp") +

    h4(text("Pointers")) + text("If you want to pass pointers that can't be ") +
    render_code("nullptr") + text(" make them references.") +
    snippet_from_file(snippets_source_code, "input-validation-pointers_0.cpp")
