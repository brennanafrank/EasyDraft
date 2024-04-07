# This document includes portions based on the python-docx-replace project, owned and maintained by Ivan Bicalho.
# Fix the placeholder located in a table inside another table


from typing import Dict, List, Dict, Any
import re

class InitialTagNotFound(Exception):
    def __init__(self, initial, end) -> None:
        super().__init__(
            f"The end tag '{end}' was found, but the initial tag '{initial}' wasn't found. "
            "Check your Word document and make sure you set the initial and end tags correctly."
        )


class EndTagNotFound(Exception):
    def __init__(self, initial, end) -> None:
        super().__init__(
            f"The initial tag '{initial}' was found, but the end tag '{end}' wasn't found. "
            "Check your Word document and make sure you set the initial and end tags correctly."
        )


class ReversedInitialEndTags(Exception):
    def __init__(self, initial, end) -> None:
        super().__init__(
            f"The end tag '{end}' appeared before the initial tag '{initial}'. "
            "Make sure you put the initial tag first."
        )


class TableIndexNotFound(Exception):
    def __init__(self, index, table_count) -> None:
        super().__init__(
            f"No table found with the index {index}. Your current document has {table_count} tables."
            "Make sure you set the right table index."
        )




class KeyChanger:
    def __init__(self, p, key, value) -> None:
        self.p = p
        self.key = key
        self.value = value
        self.run_text = ""
        self.runs_indexes: List = []
        self.run_char_indexes: List = []
        self.runs_to_change: Dict = {}

    def _initialize(self) -> None:
        run_index = 0
        for run in self.p.runs:
            self.run_text += run.text
            self.runs_indexes += [run_index for _ in run.text]
            self.run_char_indexes += [char_index for char_index, char in enumerate(run.text)]
            run_index += 1

    def replace(self) -> None:
        self._initialize()
        parsed_key_length = len(self.key)
        index_to_replace = self.run_text.find(self.key)

        for i in range(parsed_key_length):
            index = index_to_replace + i
            run_index = self.runs_indexes[index]
            run = self.p.runs[run_index]
            run_char_index = self.run_char_indexes[index]

            if not self.runs_to_change.get(run_index):
                self.runs_to_change[run_index] = [char for char_index, char in enumerate(run.text)]

            run_to_change: Dict = self.runs_to_change.get(run_index)  # type: ignore[assignment]
            if index == index_to_replace:
                run_to_change[run_char_index] = self.value
            else:
                run_to_change[run_char_index] = ""

        # make the real replace
        for index, text in self.runs_to_change.items():
            run = self.p.runs[index]
            run.text = "".join(text)




class BlockHandler:
    def __init__(self, p) -> None:
        self.p = p
        self.run_text = ""
        self.runs_indexes = []
        self.run_char_indexes = []

        run_index = 0
        for run in self.p.runs:
            self.run_text += run.text
            self.runs_indexes += [run_index for _ in run.text]
            self.run_char_indexes += [char_index for char_index, char in enumerate(run.text)]
            run_index += 1

    def replace(self, initial: str, end: str, keep_block: bool) -> None:
        initial_index = self.run_text.find(initial)
        initial_length = len(initial)
        initial_index_plus_key_length = initial_index + initial_length
        end_index = self.run_text.find(end)
        end_length = len(end)
        end_index_plus_key_length = end_index + end_length
        runs_to_change: Dict = {}

        if end_index < initial_index:
            raise ReversedInitialEndTags(initial, end)

        for index in range(initial_index, end_index_plus_key_length):
            run_index = self.runs_indexes[index]
            run = self.p.runs[run_index]
            run_char_index = self.run_char_indexes[index]

            if not runs_to_change.get(run_index):
                runs_to_change[run_index] = [char for char_index, char in enumerate(run.text)]

            run_to_change: Dict = runs_to_change.get(run_index)  # type: ignore[assignment]
            if (
                (not keep_block)
                or (index in range(initial_index, initial_index_plus_key_length))
                or (index in range(end_index, end_index_plus_key_length))
            ):
                run_to_change[run_char_index] = ""

        self._real_replace(runs_to_change)

    def clear_key_and_after(self, key: str, keep_block: bool) -> None:
        key_index = self.run_text.find(key)
        key_length = len(key)
        key_index_plus_key_length = key_index + key_length
        runs_to_change: Dict = {}

        for index in range(key_index, len(self.run_text)):
            run_index = self.runs_indexes[index]
            run = self.p.runs[run_index]
            run_char_index = self.run_char_indexes[index]

            if not runs_to_change.get(run_index):
                runs_to_change[run_index] = [char for char_index, char in enumerate(run.text)]

            run_to_change: Dict = runs_to_change.get(run_index)  # type: ignore[assignment]
            if (not keep_block) or (index in range(key_index, key_index_plus_key_length)):
                run_to_change[run_char_index] = ""

        self._real_replace(runs_to_change)

    def clear_key_and_before(self, key: str, keep_block: bool) -> None:
        key_index = self.run_text.find(key)
        key_length = len(key)
        key_index_plus_key_length = key_index + key_length
        runs_to_change: Dict = {}

        for index in range(0, key_index_plus_key_length):
            run_index = self.runs_indexes[index]
            run = self.p.runs[run_index]
            run_char_index = self.run_char_indexes[index]

            if not runs_to_change.get(run_index):
                runs_to_change[run_index] = [char for char_index, char in enumerate(run.text)]

            run_to_change: Dict = runs_to_change.get(run_index)  # type: ignore[assignment]
            if (not keep_block) or (index in range(key_index, key_index_plus_key_length)):
                run_to_change[run_char_index] = ""

        self._real_replace(runs_to_change)

    def _real_replace(self, runs_to_change: dict):
        # make the real replace
        for index, text in runs_to_change.items():
            run = self.p.runs[index]
            run.text = "".join(text)




class Paragraph:
    @staticmethod
    def get_all(doc) -> List[Any]:
        paragraphs = []
        # Extend the list of paragraphs from the main document and headers/footers.
        for item in [doc] + [section.header for section in doc.sections] + [section.footer for section in doc.sections]:
            paragraphs.extend(Paragraph._get_paragraphs(item))
        return paragraphs

    @staticmethod
    def _get_paragraphs(item: Any) -> Any:
        # Yield paragraphs from the current item (document, header, footer).
        yield from item.paragraphs

        # Get paragraphs from tables, including nested tables.
        for table in item.tables:
            for row in table.rows:
                for cell in row.cells:
                    # Recursively get paragraphs from tables within cells.
                    yield from Paragraph._get_paragraphs(cell)

    def __init__(self, p) -> None:
        self.p = p

    def delete(self) -> None:
        paragraph = self.p._element
        paragraph.getparent().remove(paragraph)
        paragraph._p = paragraph._element = None

    def contains(self, key) -> bool:
        return key in self.p.text

    def startswith(self, key) -> bool:
        return str(self.p.text).strip().startswith(key)

    def endswith(self, key) -> bool:
        return str(self.p.text).strip().endswith(key)

    def replace_key(self, key, value) -> None:
        if key in self.p.text:
            self._simple_replace_key(key, value)
            if key in self.p.text:
                self._complex_replace_key(key, value)

    def replace_block(self, initial, end, keep_block) -> None:
        block_handler = BlockHandler(self.p)
        block_handler.replace(initial, end, keep_block)

    def clear_tag_and_before(self, key, keep_block) -> None:
        block_handler = BlockHandler(self.p)
        block_handler.clear_key_and_before(key, keep_block)

    def clear_tag_and_after(self, key, keep_block) -> None:
        block_handler = BlockHandler(self.p)
        block_handler.clear_key_and_after(key, keep_block)

    def get_text(self) -> str:
        return self.p.text

    def _simple_replace_key(self, key, value) -> None:
        # try to replace a key in the paragraph runs, simpler alternative
        for run in self.p.runs:
            if key in run.text:
                run.text = run.text.replace(key, value)

    def _complex_replace_key(self, key, value) -> None:
        # complex alternative, which check all broken items inside the runs
        while key in self.p.text:
            # if the key appears more than once in the paragraph, it will replaced all
            key_changer = KeyChanger(self.p, key, value)
            key_changer.replace()




__all__ = ["docx_replace", "docx_blocks", "docx_remove_table"]


def docx_replace(doc, **kwargs: str) -> None:
    """
    Replace all the keys in the word document with the values in the kwargs

    ATTENTION: The required format for the keys inside the Word document is: ${key}

    Example usage:
        Word content = "Hello ${name}, your phone is ${phone}, is that okay?"

        doc = Document("document.docx")  # python-docx dependency

        docx_replace(doc, name="Ivan", phone="+55123456789")

    More information: https://github.com/ivanbicalho/python-docx-replace
    """
    for key, value in kwargs.items():
        key = f"${{{key}}}"
        for p in Paragraph.get_all(doc):
            paragraph = Paragraph(p)
            paragraph.replace_key(key, str(value))


def docx_blocks(doc: Any, **kwargs: bool) -> None:
    """
    Keep or remove blocks in the word document

    ATTENTION: The required format for the block keys inside the Word document are: <key> and </key>
        <key> stands for initial block
        </key> stands for end block
        Everything inside the block will be removed or not, depending on the configuration

    Example usage:
        Word content = "Hello<name> Ivan</name>, are you okay?"

        doc = Document("document.docx")  # python-docx dependency

        docx_handle_blocks(doc, name=False)
        result = "Hello, are you okay?"

        docx_handle_blocks(doc, name=True)
        result = "Hello Ivan, are you okay?"

    More information: https://github.com/ivanbicalho/python-docx-replace
    """
    for key, keep_block in kwargs.items():
        initial = f"<{key}>"
        end = f"</{key}>"

        result = _handle_blocks(doc, initial, end, keep_block)
        while result:  # if the keys appear more than once, it will replace all
            result = _handle_blocks(doc, initial, end, keep_block)

        # end tags can exists alone in the document
        # this function just make sure that if it's the case, raise an error
        _search_for_lost_end_tag(doc, initial, end)


def docx_remove_table(doc: Any, index: int) -> None:
    """
    Remove a table from your Word document by index

    Example usage:
        docx_remove_table(doc, 0)  # it will remove the first table

    If the table index wasn't found, an error will be raised

    ATTENTION:
        The table index works exactly like any indexing property. It means if you
        remove an index, it will affect the other indexes. For example, if you want
        to remove the first two tables, you can't do like this:
            - docx_remove_table(doc, 0)
            - docx_remove_table(doc, 1)
        You should instead do like this:
            - docx_remove_table(doc, 0)
            - docx_remove_table(doc, 0)
    """
    try:
        table = doc.tables[index]
        table._element.getparent().remove(table._element)
    except IndexError:
        raise TableIndexNotFound(index, len(doc.tables))


def docx_get_keys(doc: Any) -> List[str]:
    """
    Search for all keys in the Word document and return a list of unique elements

    ATTENTION: The required format for the keys inside the Word document is: ${key}

    For a document with the following content: "Hello ${name}, is your phone ${phone}?"
    Result example: ["name", "phone"]
    """
    result = set()  # unique items
    for p in Paragraph.get_all(doc):
        paragraph = Paragraph(p)
        matches = re.finditer(r"\$\{([^{}]+)\}", paragraph.get_text())
        for match in matches:
            result.add(match.groups()[0])
    return list(result)


def _handle_blocks(doc: Any, initial: str, end: str, keep_block: bool) -> bool:
    # The below process is a little bit complex, so I decided to comment each step
    look_for_initial = True
    for p in Paragraph.get_all(doc):
        paragraph = Paragraph(p)
        if look_for_initial:
            if paragraph.contains(initial):
                look_for_initial = False  # initial tag found, next search will be for the end tag
                if paragraph.contains(end):
                    # if the initial and end tag are in the same paragraph, treat them together
                    paragraph.replace_block(initial, end, keep_block)
                    return True  # block completed, returns
                else:
                    # the current paragraph doesn't have the end tag
                    if paragraph.startswith(initial):
                        # if the paragraph starts with the initial tag, we can clear the tag if is to keep_block
                        # otherwise we can delete the entire paragraph because the end tag is not here
                        if keep_block:
                            paragraph.clear_tag_and_before(initial, keep_block)
                        else:
                            paragraph.delete()
                        continue
                    else:
                        # if the paragraph doesn't start with the initial tag, we cannot delete the entire
                        # paragraph, we have to clear the tag and remove the content right after (if not keep_block)
                        paragraph.clear_tag_and_after(initial, keep_block)
                        continue
        else:
            # we are looking for the end tag as the initial tag was found and treated before
            if paragraph.contains(end):
                # end tag found in this paragraph
                if paragraph.endswith(end):
                    # if the paragraph ends with the end tag we can clear the tag if is to keep_block
                    # otherwise we can delete the entire paragraph
                    if keep_block:
                        paragraph.clear_tag_and_after(end, keep_block)
                    else:
                        paragraph.delete()
                    return True  # block completed, returns
                else:
                    # if the paragraph doesn't end with the end tag, we cannot delete the entire
                    # paragraph, we have to clear the tag and remove the content right before (if not keep_block)
                    paragraph.clear_tag_and_before(end, keep_block)
                    return True  # block completed, returns
            else:
                # paragraph doesn't have the end key, that means there is no tags here. In this case,
                # we can remove the entire paragraph if not keep_block, otherwise do nothing
                if not keep_block:
                    paragraph.delete()
                continue
    if look_for_initial:
        # if the initial tag wasn't found, the block doesn't exist in the Word document
        return False  # block completed, returns
    else:
        # if the initial tag was found, but not end tag, raise an error
        raise EndTagNotFound(initial, end)


def _search_for_lost_end_tag(doc: Any, initial: str, end: str) -> None:
    for p in Paragraph.get_all(doc):
        paragraph = Paragraph(p)
        if paragraph.contains(end):
            raise InitialTagNotFound(initial, end)
