<?cs # The default search box that goes in the header ?><?cs
def:default_search_box() ?>
  <div id="search" >
      <div id="searchForm">
          <form accept-charset="utf-8" class="gsc-search-box"
                onsubmit="return submit_search()">
            <table class="gsc-search-box" cellpadding="0" cellspacing="0"><tbody>
                <tr>
                  <td class="gsc-input">
                    <input id="search_autocomplete" class="gsc-input round minimal" type="text" size="33" autocomplete="off"
                      title="search developer docs" name="q"
                      value="search developer docs"
                      onFocus="search_focus_changed(this, true)"
                      onBlur="search_focus_changed(this, false)"
                      onkeydown="return search_changed(event, true, '<?cs var:toroot?>')"
                      onkeyup="return search_changed(event, false, '<?cs var:toroot?>')" />
                  <div id="search_filtered_div" class="no-display">
                      <table id="search_filtered" cellspacing=0>
                      </table>
                  </div>
                  </td>

                </tr></tbody>
              </table>
          </form>
      </div><!-- searchForm -->
  </div><!-- search --><?cs
/def ?>
