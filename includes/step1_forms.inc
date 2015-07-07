<?php
/**
 * @file
 *
 */

/**
 * Form allowing breeders to set the crosses to be made
 * for a given crossing block.
 * @TODO: Extract data from Chado where necessary
 *
 * Implements hook_form().
 */
function cross_manage_crossingblock_set_cross_parents_form($form, &$form_state) {

  // DEFAULTS:
  //--------------------------
  // If the crossing block is already set
  if (sizeof($form_state['build_info']['args']) >=3 ) {
    // Crossing block
    $cb_set = TRUE;
    $species = str_replace('-',' ',$form_state['build_info']['args'][0]);
    $cb_year = $form_state['build_info']['args'][1];
    $cb_season = $form_state['build_info']['args'][2];

    // Set Page Title
    drupal_set_title(format_string('Plan Crosses: @species, @season @year Crossing Block', array(
        '@species' => $species,
        '@year' => $cb_year,
        '@season' => $cb_season,
    )));

    $highlight_options = array(
      'market_class' => 'Market Class'
    );

    // What to highlight the rows based on
    $highlight_detail = 'market_class';

    $marker_data_avail = FALSE;
    if (isset($form_state['build_info']['args']['markers_screened'])) {
      $marker_data_avail = TRUE;
    }
  }
  else {
    $cb_set = FALSE;
    $species = NULL;
    $cb_year = date("Y");
    $cb_season = NULL;
  }

  // Add Themeing:
  //--------------------------
  $crossmanage_path = drupal_get_path('module', 'cross_manage');
  $form['#attached'] = array(
    'css' => array(
      'type' => 'file',
      'data' => $crossmanage_path . '/theme/css/cross_manage.css',
    ),
  );
  $form['#attached']['js'][] = $crossmanage_path . '/theme/js/CrossManage_highlightErrorRow.js';


  // Add Shepherd Site Tour
  //--------------------------
  $shepherd_path = libraries_get_path('shepherd');
  $form['#attached']['js'][] = $shepherd_path . '/shepherd.min.js';
  $form['#attached']['js'][] = $crossmanage_path . '/theme/js/CrossManage_Parentsform_ShepherdTour.js';
  $form['#attached']['css'][] = $shepherd_path .'/css/shepherd-theme-arrows.css';
  $form['#attached']['css'][] = array(
    'data' => $crossmanage_path . '/theme/css/crossmanage_shepherd_tour.css',
    'weight' => 10
  );

  // Add site tour link.
  //--------------------------
  $form['start_tour'] = array(
    '#type' => 'markup',
    '#markup' => theme('cross_manage_shepherd_start_tour_link')
  );

  // FORM PROPER:
  //--------------------------
/**
  $form['highlight'] = array(
    '#type' => 'fieldset',
    '#title' => 'Row Highlight'
  );

  $form['highlight']['highlight_select'] = array(
    '#type' => 'select',
    '#title' => 'Highlight Row',
    '#description' => 'Select the detail you wish to highlight the rows based on.',
    '#options' => $highlight_options,
    '#default_value' => $highlight_detail,
    '#prefix' => '<span class="cross-parents highlight highlight-select">',
    '#suffix' => '</span>'
  );

  $form['highlight']['highlight_key'] = array(
    '#type' => 'markup',
    '#markup' => "<table>
                    <caption><strong>Highlight Colour Key</strong></caption>
                    <tr><td class='highlight-key Small-Red'></td><td>Small Red</td></tr>
                    <tr><td class='highlight-key French-Green'></td><td>French Green</td></tr>
                  </table>",
    '#prefix' => '<span class="cross-parents highlight highlight-key">',
    '#suffix' => '</span>'
  );
*/

  // Begin parents table
  $parents_wrapper = 'parents-table';
  $form['parents'] = array(
    '#type' => ($cb_set) ? 'markup' : 'hidden',
    '#prefix' => '<div id="' . $parents_wrapper . '">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
    '#theme' => 'cross_manage_crossingblock_parents',
  );

  // We need to grab the parents for this crossingblock.
  // Always pull the parents from the database since they should be saved on
  // addition of a parent. This is consistent and reliable.
  $parents = db_query("
    SELECT p.*, s.name, s.uniquename, cs.nid,
      self.value as breeder_notes, instruct.value as instructions,
      (SELECT array_agg(paternal_parent_id) FROM cross_manage_planned_crosses WHERE maternal_parent_id=p.cb_parent_id) as cross_to
    FROM {cross_manage_parents} p
    LEFT JOIN chado.stock s ON s.stock_id=p.stock_id
    LEFT JOIN {chado_stock} cs ON cs.stock_id = p.stock_id
    LEFT JOIN {cross_manage_notes} self ON self.cb_parent_id=p.cb_parent_id AND self.type='breeder_selfnote'
    LEFT JOIN {cross_manage_notes} instruct ON instruct.cb_parent_id=p.cb_parent_id AND instruct.type='fieldlab_instruction'
    WHERE cb_year=:year AND cb_season=:season AND cb_species=:species
    ORDER BY parent_number",
    array(':year' => $cb_year, ':season' => $cb_season, ':species' => $species))->fetchAll();
  usort($parents, 'cross_manage_sort_parents_by_pnumber');
  $num_rows = sizeof($parents);

  // Set the cross-to options - first value is blank so the drop-down is initially blank
  $parent_options = array(0 => '');
  foreach ($parents as $p) {
    $parent_options[$p->cb_parent_id] = $p->name .' (P'.$p->parent_number.')';
  }
  asort($parent_options);


  // Use For loop to ensure we always get one more row than options.
  // We didn't use +1 on existing options b/c there is an add-row element
  // for the button which effectively adds our +1.
  // ASSUMPTION: You will not add any elements to $form['parents'] other than
  // the rows and the add parent button. If you do,
  // (ie: $form['parents']['another button']) you will get two empty rows.

  $max_pnum = 0;
  for ($i=1; $i < $num_rows; $i++) {

    // Either grab the existing options from the form state
    if (isset($parents[$i])) {
      $has_form_state = (isset($form_state['values']['parents'])) ? TRUE : FALSE;
      $pid = $parents[$i]->cb_parent_id;

      $pnum = ($has_form_state) ? $form_state['values']['parents'][$pid]['pnum'] : $parents[$i]->parent_number;
      $line = ($has_form_state) ? $form_state['values']['parents'][$pid]['line'] : $parents[$i]->name;
      $source = ($has_form_state) ? $form_state['values']['parents'][$pid]['source'] : $parents[$i]->seed_source;
      $breeder_notes = ($has_form_state) ? $form_state['values']['parents'][$pid]['notes']['breeder_notes'] : $parents[$i]->breeder_notes;
      $instructions = ($has_form_state) ? $form_state['values']['parents'][$pid]['notes']['instructions'] : $parents[$i]->instructions;

      $cross_to = array();
      if (!empty($parents[$i]->cross_to)) {
        if (!is_array($parents[$i]->cross_to)) {
          $parents[$i]->cross_to = explode("," , trim($parents[$i]->cross_to, '{}'));
        }
        $cross_to = $parents[$i]->cross_to;
      }
      $cross_to = ($has_form_state AND $form_state['values']['parents'][$pid]['cross_to'][0] != 0) ? $form_state['values']['parents'][$pid]['cross_to'] : $cross_to;

      $details = 'Germplasm Page';
      if (isset($parents[$i]->nid)) {
        $details = l($details, 'node/'.$parents[$i]->nid, array('attributes' => array('target' => '_blank')));
      }

      // Keep track of the maximum parent number in order to autofill the last row.
      $max_pnum = ($pnum > $max_pnum) ? $pnum : $max_pnum;
    }
    // OR if this is the last row then there are no existing values
    // so set blank defaults.
    else {
      $pid = 'new';
      $pnum = $max_pnum + 1;
      $line = '';
      $source = '';
      $cross_to = array();
      $details = '';
      $line_not_valid = FALSE;
    }

    $highlight_class = 'no-highlight';
    if (isset($opt['details'][$highlight_detail])) {
      $highlight_class = str_replace(' ','-', $opt['details'][$highlight_detail]['value']);
    }

    $form['parents'][$pid] = array(
      '#type' => 'markup',
      //'#prefix' => '<span class="cross-parents row '.$highlight_class.'">',
      //'#suffix' => '</span>'
    );
    if ($i == ($num_rows - 1)) {
      $form['parents'][$pid]['#prefix'] = '<span class="cross-parents row last">';
    }

    $form['parents'][$pid]['highlight_class'] = array(
      '#type' => 'item',
      '#value' => $highlight_class
    );

    $form['parents'][$pid]['pnum'] = array(
      '#type' => 'textfield',
      //'#title' => 'Parent Number',
      '#default_value' => $pnum,
      '#prefix' => '<span class="pre-label">P</span>',
      '#size' => 2,
      '#required' => TRUE,
    );

    $form['parents'][$pid]['cb_parent_id'] = array(
      '#type' => 'hidden',
      '#value' => $pid
    );

    $form['parents'][$pid]['line'] = array(
      '#type' => 'textfield',
      //'#title' => 'Line',
      '#default_value' => $line,
      '#description' => $details,
      '#size' => 35,
      '#required' => TRUE,
      '#autocomplete_path' => 'tripal_ajax/cross_manage/stock_name/'.$form_state['build_info']['args'][0]
    );

    $form['parents'][$pid]['source'] = array(
      '#type' => 'textfield',
      //'#title' => 'Source',
      '#default_value' => $source,
      '#size' => 35,
      '#required' => TRUE,
    );

    $form['parents'][$pid]['notes'] = array(
      '#type' => 'markup',
    );

    // We want to expand the size of the textarea based on the number of lines
    // up to a max of 3 lines. First we use a simple regex to count the number
    // of user defined lines. However, a user could type a paragraph that spans
    // multiple lines (not detected by regex) so we also use a character count
    // if the number of lines is determined to be one or less.
    $num_lines = preg_match_all('/(\r|\n|\<br\/\>|\<b\/\>|\<br \/\>)/', $breeder_notes);
    $num_lines = ($num_lines <= 1) ? ceil(strlen($breeder_notes)/75) : $num_lines;
    $length = ($num_lines >= 3) ? 3 : $num_lines;
    $length = ($length == 0) ? 1 : $length;

    $form['parents'][$pid]['notes']['breeder_notes'] = array(
      '#type' => 'textarea',
      '#attributes' => array('placeholder' => 'Breeder Notes'),
      '#rows' => $length,
      '#default_value' => $breeder_notes,
    );

    // Used the same logic as described for the number of rows for the
    // Breeder's notes textarea above.
    $num_lines = preg_match_all('/(\r|\n|\<br\/\>|\<b\/\>|\<br \/\>)/', $instructions);
    $num_lines = ($num_lines <= 1) ? ceil(strlen($instructions)/75) : $num_lines;
    $length = ($num_lines >= 3) ? 3 : $num_lines;
    $length = ($length == 0) ? 1 : $length;

    $form['parents'][$pid]['notes']['instructions'] = array(
      '#type' => 'textarea',
      '#attributes' => array('placeholder' => 'Instructions'),
      '#rows' => $length,
      '#default_value' => $instructions,
    );

    $cross_to_wrapper = 'cross-to-' . $pid;
    $form['parents'][$pid]['cross_to'] = array(
      '#type' => 'item',
      '#prefix' => '<div id="' . $cross_to_wrapper . '">',
      '#suffix' => '</div>',
      //'#title' => 'Cross To',
    );

    // Add a number of cross to dropdowns to allow breeders to specify what
    // parents they want to cross with the current line
    // Ajax is used to ensure that there is always one more select box
    // than there are parents selected so that breeders can add more parents
    // to cross with.

    // Determine how many select boxes we need.
    // We don't need to add one because we are using the 0-index where
    // size starts at 1.
    $num_crosses = (!empty($cross_to)) ? sizeof($cross_to) : 0;

    for ($j = 0; $j <= $num_crosses; $j++) {
      // Determine our default value for the next select box.
      if (isset($cross_to[$j])) {
        // Default value is what has been previously entered
        $cross_to_default = $cross_to[$j];
        // If the final box is already empty, don't iterate through the loop a final time
        // Otherwise, another empty box would be added.
        if (empty($cross_to[$j]) AND ($j == ($num_crosses - 1))) {
          $num_crosses--;
        }
      }
      else { // This is a brand new select box, so make the default value empty.
        $cross_to_default = 0;
      }

      // Create the select box
      $form['parents'][$pid]['cross_to'][] = array(
        '#type' => 'select',
        '#options' => $parent_options,
        '#default_value' => $cross_to_default, // @carolyn: Drupal was fixing broken code!
        '#ajax' => array(
          'callback' => 'ajax_parents_cross_to_callback',
          'wrapper' => $cross_to_wrapper,
        ),
      );
    }
  }

  // Some ajax has been commented out that was meant to add a new row to the
  // parents table without losing any information that has already been entered.
  // This is currently being done with a form rebuild instead.
  $form['parents']['add-row'] = array(
    '#type' => 'submit',
    '#value' => 'Add Parent',
    '#prefix' => '<span class="cross-parents add-more">',
    '#suffix' => '</span>',
  );

  $form['submit-cb'] = array(
    '#type' => 'submit',
    '#value' => 'Submit Crossing Plans',
  );

  return $form;
}

/**
  * Form validation for breeders to set crosses.
  * Ensures a cross cannot occur with itself, nor can 2+ identical crosses occur
  */
function cross_manage_crossingblock_set_cross_parents_form_validate($form, &$form_state) {

  // We are going to need to validate for duplicate line/source parents. In
  // order to avoid a nested loop situation we are goign to keep track
  // of parent line/source combinations in the following array and then check
  // at the end for duplication.
  $stocksource_count = array();
  $stocksource_pid = array();

  // Look at each row in the same manner.
  foreach ($form_state['values']['parents'] as $pid => $row) {

    // VALIDATE: ensure that the line entered is valid and that the name
    // entered can be unique attached to a stock_id.
    // Note that this shouldn't be empty because it's marked required
    // but we are checking anyway since a database query is involved
    // and we all know there are sometimes #DrupalWTF moments ;).
    if (!empty($form_state['values']['parents'][$pid]['line'])) {

      $matching_stock_id = cross_manage_get_stock_by_name($form_state['values']['parents'][$pid]['line'], $form_state['build_info']['args'][0]);
      if (!$matching_stock_id) {
        form_set_error("parents][$pid][line", "No Germplasm exists with that unique name.");
      }
      elseif (is_numeric($matching_stock_id)) {
        $form_state['values']['parents'][$pid]['stock_id'] = $matching_stock_id;
      }
      else {
        $ids = array();
        foreach ($matching_stock_id as $ms_id) $ids[] = $ms_id->stock_id;
        $records = db_query('
          SELECT cs.nid, s.uniquename
          FROM {chado_stock} cs
          LEFT JOIN chado.stock s ON s.stock_id=cs.stock_id
          WHERE cs.stock_id IN (:ids)',
          array(':ids' => $ids));
        $links = array();
        foreach ($records as $r) {
          $links[] = l($r->uniquename, 'node/'.$r->nid);
        }
        $msg = t('The name, :name, is not unique (!links), try using the accession of the germplasm you meant.',
          array(':name' => $form_state['values']['parents'][$pid]['line'], '!links' => implode(', ', $links)));
        form_set_error("parents][$pid][line", $msg);
      }

      // Further prerparation for checking if two parents have the same
      // line/source. Here we are essentially flattening the form_state keeping
      // only the information we are interested in.
      if (!empty($form_state['values']['parents'][$pid]['source']) AND isset($form_state['values']['parents'][$pid]['stock_id'])) {
        $stocksource_key = $form_state['values']['parents'][$pid]['stock_id'] . '|' . $form_state['values']['parents'][$pid]['source'];
        $stocksource_count[$stocksource_key] = (isset($stocksource_count[$stocksource_key])) ? $stocksource_count[$stocksource_key]+1 : 1;
        $stocksource_pid[$pid] = $stocksource_key;
      }
    }

    // Specifically the "Cross To" elements...
    // We assume at this point that line is present and valid.
    if (isset($form_state['values']['parents'][$pid]['cross_to'])) {

      // VALIDATE: Ensure the user hasn't crossed the parent to itself.
      $duplicate_field_id = array_search($pid, $form_state['values']['parents'][$pid]['cross_to']);
      if (is_numeric($duplicate_field_id)) {

        // We only want to show the warning on ajax if the triggering element
        // is in the same row as the offending cross. The message becomes
        // confusing if shown in the wrong row.
        if (isset($form_state['triggering_element']['#ajax']) AND $form_state['triggering_element']['#parents'][1] == $pid) {
          $msg = t(
            'You have indicated you want to cross :pname with itself (See :index paternal parent). Please explain further in the "Instructions" box to the right.',
            array(
              ':pname' => $form_state['values']['parents'][$pid]['line'],
              ':pnum' => $form_state['values']['parents'][$pid]['pnum'],
              ':index' => cross_manage_format_number_ordinal($duplicate_field_id + 1)
            ));
          drupal_set_message($msg, 'warning');
        }
        // We always want to show it if a button was pressed.
        elseif (isset($form_state['clicked_button'])) {
          $msg = t(
            'P:pnum: You have indicated you want to cross :pname with itself (See :index paternal parent). Please explain further in the associated "Instructions" box.',
            array(
              ':pname' => $form_state['values']['parents'][$pid]['line'],
              ':pnum' => $form_state['values']['parents'][$pid]['pnum'],
              ':index' => cross_manage_format_number_ordinal($duplicate_field_id + 1)
            ));
          drupal_set_message($msg, 'warning');
        }
      }

      // VALIDATE: The user shouldn't make the sample cross 2X.
      // LOGIC: Since the values of the cross_to elements are parent numbers,
      // we can check for duplicate parent number by counting the occurances
      // of each value. If the array of counts is smaller than the array of
      // crosses then one of them must have been duplicated. Furthermore, we can
      // now search through the counts to find the duplicated parent number.
      $cross_count = array_count_values($form_state['values']['parents'][$pid]['cross_to']);
      if (sizeof($form_state['values']['parents'][$pid]['cross_to']) !== sizeof($cross_count)) {

        // Search through the counts for any duplicated parent number. Since the
        // parent number is the key we can use array_keys to get a list of
        // duplicated parent numbers.
        // We use array_filter with a callback to ensure we catch instances
        // where the user has selected the same parent 2+ times.
        $duplicated_pids = array_filter($cross_count, 'cross_manage_filter_keep_2plus', ARRAY_FILTER_USE_BOTH);

        // For each duplicated parent number, alert the user.
        foreach($duplicated_pids as $dup_pid => $count) {

          if ($dup_pid !== 0) {
            // Now we need a list of the element indices for the duplicated parent
            // number. We can use array_keys again on the cross_to elements
            // to find the indicies where the value is the duplicated parent number.
            $field_ids = array_keys($form_state['values']['parents'][$pid]['cross_to'], $dup_pid);


            // Set an error on the duplicate fields.
            $error_msg_1st = 'Duplicate Cross :maternal x :paternal.';
            $error_msg_rm = $error_msg_1st . ' Remove the :index paternal parent.';
            $error_tokens = array(
              ':maternal' => $form_state['values']['parents'][$pid]['line'],
              ':paternal' => $form_state['values']['parents'][$dup_pid]['line'],
              ':count' => $count,
              ':index' => 'duplicate'
            );

            $fi = 0;
            foreach($field_ids as $id) {
              $fi++;
              if ($fi == 1) {
                $message = t($error_msg_1st, $error_tokens);
              }
              else {
                $error_tokens[':index'] = cross_manage_format_number_ordinal($id + 1);
                $message = t($error_msg_rm, $error_tokens);
              }
              form_set_error('parents][' . $pid . '][cross_to][' . $id, $message);


              // For some reason we don't see form errors on AJAX but we do see
              // drupal messages. Therefore, warn the user on AJAX about the
              // duplication.
              if (isset($form_state['triggering_element']['#ajax'])) {
                drupal_set_message($message, 'warning');;
              }
            }
          }
        }

      }

    }
  }

  // VALIDATE: Ensure that two parents don't have the same stock_id & source
  // as this would cause a db (PDO) error on insert/update.
  // We use the custom stocksource_count array generated above to keep track of
  // the number of each stock/source combination making it easy to check once,
  // down here, how many duplications are present. This cannot be done within
  // the loop because we would get multiple messages if the combination
  // was present 3+ times.
  $dup_stocksources = array_filter($stocksource_count, 'cross_manage_filter_keep_2plus', ARRAY_FILTER_USE_BOTH);
  foreach ($dup_stocksources as $dup_combo => $count) {
    $dup_pids = array_keys($stocksource_pid, $dup_combo);
    foreach ($dup_pids as $pid) {
      form_set_error('parents]['.$pid.'][line',
        t(':name (P:pnum) has the same line and source as :count other parents.',
          array(':name' => $form_state['values']['parents'][$pid]['line'], ':pnum' => $form_state['values']['parents'][$pid]['pnum'],':count' => $count-1)));
    }
  }

}

/**
 * Form submission for breeders to either submit the crossing block in its current state
 * or add another row for a new parent.
 *
 * Implements hook_form_submit().
 */
function cross_manage_crossingblock_set_cross_parents_form_submit(&$form, &$form_state) {

  $form_state['rebuild'] = TRUE;

  // Regardless we want to save all the changes.
  foreach (element_children($form_state['values']['parents']) as $pid) {
    if ($pid != 'add-row') {

      // Table: cross_manage_parents.
      // Keeps track of basic details for each parent.
      $parent_record = array(
        'stock_id' => $form_state['values']['parents'][$pid]['stock_id'],
        'cb_year' => $form_state['build_info']['args'][1],
        'cb_season' => $form_state['build_info']['args'][2],
        'seed_source' => $form_state['values']['parents'][$pid]['source']
      );

      // If there is no cb_parent_id then this is a new record.
      if (empty($form_state['values']['parents'][$pid]['cb_parent_id'])) {
        drupal_write_record('cross_manage_parents', $parent_record);

        if (isset($parent_record['cb_parent_id'])) {
          $form_state['values']['parents'][$pid]['cb_parent_id'] = $parent_record['cb_parent_id'];
        }
        else {
          // ERROR: unable to save parent.
        }
      }
      else {
        $parent_record['cb_parent_id'] = $form_state['values']['parents'][$pid]['cb_parent_id'];
        drupal_write_record('cross_manage_parents', $parent_record, 'cb_parent_id');
      }

      if (isset($parent_record['cb_parent_id'])) {

        // Table: cross_manage_planned_crosses.
        // Keeps track of each cross planned. In other words stores each
        // maternal/paternal parent cross specified in the "Cross To" drop-downs.
        foreach ($form_state['values']['parents'][$pid]['cross_to'] as $pat_pid) {
          $cross_record = array();

          if ($pat_pid != 0) {
            $cross_record = array(
              'maternal_parent_id' => $parent_record['cb_parent_id'],
              'paternal_parent_id' => $pat_pid
            );

            $exists = db_query('
              SELECT cb_cross_id
              FROM {cross_manage_planned_crosses}
              WHERE maternal_parent_id=:mat AND paternal_parent_id=:pat',
              array(
                ':mat' => $cross_record['maternal_parent_id'],
                ':pat' => $cross_record['paternal_parent_id']
            ))->fetchField();
            if ($exists) {
              $cross_record['cb_cross_id'] = $exists;
              drupal_write_record('cross_manage_planned_crosses', $cross_record, 'cb_cross_id');
            }
            else {
              drupal_write_record('cross_manage_planned_crosses', $cross_record);
            }
          }
        }

        // Delete any removed planned crosses.
        // Logic: Restricted to the maternal_parent_id, delete any crosses
        // where the paternal parent is not in the cross_to list.
        if (sizeof($form_state['values']['parents'][$pid]['cross_to']) > 1) {
          db_query('DELETE FROM {cross_manage_planned_crosses} WHERE maternal_parent_id=:mat AND paternal_parent_id NOT IN (:pats)',
              array(':mat' => $parent_record['cb_parent_id'], ':pats' => $form_state['values']['parents'][$pid]['cross_to']))->fetchAll();
        }

        // Table: cross_manage_notes.
        // Keeps track of breeder's notes and instructions.
        if (!empty($form_state['values']['parents'][$pid]['notes']['breeder_notes'])) {
          $selfnote_record = array(
            'cb_parent_id' => $parent_record['cb_parent_id'],
            'type' => 'breeder_selfnote',
            'value' => $form_state['values']['parents'][$pid]['notes']['breeder_notes']
          );

          $exists = db_query('
            SELECT true
            FROM {cross_manage_notes}
            WHERE cb_parent_id=:pid AND type=:type',
            array(
              ':pid' => $pid,
              ':type' => 'breeder_selfnote'
          ))->fetchField();
          if ($exists) {
            drupal_write_record('cross_manage_notes', $selfnote_record, array('cb_parent_id','type'));
          }
          else {
            drupal_write_record('cross_manage_notes', $selfnote_record);
          }
        }
        if (!empty($form_state['values']['parents'][$pid]['notes']['instructions'])) {
          $instruct_record = array(
            'cb_parent_id' => $parent_record['cb_parent_id'],
            'type' => 'fieldlab_instruction',
            'value' => $form_state['values']['parents'][$pid]['notes']['instructions']
          );

          $exists = db_query('
            SELECT true
            FROM {cross_manage_notes}
            WHERE cb_parent_id=:pid AND type=:type',
            array(
              ':pid' => $pid,
              ':type' => 'fieldlab_instruction'
          ))->fetchField();
          if ($exists) {
            drupal_write_record('cross_manage_notes', $instruct_record, array('cb_parent_id','type'));
          }
          else {
            drupal_write_record('cross_manage_notes', $instruct_record);
          }
        }
      }
    }
  }

  if ($form_state['clicked_button']['#value'] == 'Submit Crossing Plans') {
    $form_state['rebuild'] = FALSE;
    drupal_set_message('Successfully submitted crossing plans');
  }

}

/**
function ajax_parents_add_row_callback($form, $form_state) {
  return $form['parents'];
}
*/

/**
 * Callback for ajax-enabled select boxes
 * Retrieves the pnum from the row that was "triggered" and returns the cross-to section
 * of the form for that row.
 */
function ajax_parents_cross_to_callback($form, $form_state) {
  $pnum = $form_state['triggering_element']['#array_parents'][1];
  return $form['parents'][$pnum]['cross_to'];
}

/**
 * Implements hook_form().
 * Form: Plan Crosses: F1's
 *
 * Form allowing breeders to indicate which germplasm should be crossed to the
 * F1s resulting from the previous crossing block. The ability to provide
 * additional instructions, such as seed designated for grow-outs and crosses,
 * is also available.
 */
function cross_manage_crossingblock_set_cross_f1s_form($form, &$form_state) {

  // Get the year/season for the PREVIOUS corssing block since that's
  // where the record for the F1's would be.
  // To determine previous crossing block, we need to choose the previous
  // season and if the current season is spring then choose winter of the
  // previous year.
  $seasons = array('Spring', 'Summer', 'Fall', 'Winter');
  $curr_season_key = array_search($form_state['build_info']['args'][2], $seasons);
  if ($curr_season_key > 0) {
    $prev_cb_season = $seasons[$curr_season_key - 1];
    $prev_cb_year = $form_state['build_info']['args'][1];
  } else {
    $prev_cb_season = 'Winter';
    $prev_cb_year = $form_state['build_info']['args'][1] - 1;
  }

  // The following query pulls all F1s from the PREVIOUS crossing block.
  // Where this becomes difficult is that the breeders don't put
  // in a crossing block every querter so if the query below returns 0 then
  // you need to grab for those for 2 crossing blocks ago.
  $f1s = chado_query("
    SELECT
      s.*,
      org.genus,
      org.species,
      type.name as type_name,
      mats.name||'/'||pats.name as pedigree,
      num_seeds.value as num_seeds
    FROM {stock} s
    LEFT JOIN {stockprop} cby ON s.stock_id=cby.stock_id AND cby.type_id=:cb_year_id
    LEFT JOIN {stockprop} cbs ON s.stock_id=cbs.stock_id AND cbs.type_id=:cb_season_id
    LEFT JOIN {organism} org ON org.organism_id=s.organism_id
    LEFT JOIN {cvterm} type ON type.cvterm_id=s.type_id
    LEFT JOIN {stock_relationship} mat ON mat.object_id=s.stock_id AND mat.type_id=:mat_type_id
    LEFT JOIN {stock} mats ON mats.stock_id=mat.subject_id
    LEFT JOIN {stock_relationship} pat ON pat.object_id=s.stock_id AND pat.type_id=:pat_type_id
    LEFT JOIN {stock} pats ON pats.stock_id=pat.subject_id
    LEFT JOIN {stockprop} num_seeds ON num_seeds.stock_id=s.stock_id AND num_seeds.type_id=:seeds_type_id
    WHERE cby.value=:cb_year AND cbs.value=:cb_season AND org.genus=:genus
    ORDER BY s.name ASC",
    array(
      'mat_type_id' => '3632',
      'pat_type_id' => '3633',
      'seeds_type_id' => '3973',
      'cb_year_id' => '3663',
      'cb_season_id' => '3664',
      'genus' => $form_state['build_info']['args'][0],
      'cb_year' => $prev_cb_year,
      'cb_season' => $prev_cb_season,
    )
  )->fetchAll();

  // Set Page Title
  $species = str_replace('-',' ',$form_state['build_info']['args'][0]);
  $cb_year = $form_state['build_info']['args'][1];
  $cb_season = $form_state['build_info']['args'][2];
  drupal_set_title(format_string('Plan Crosses with F1s from: @species, @season @year Crossing Block', array(
      '@species' => $species,
      '@year' => $cb_year,
      '@season' => $cb_season,
  )));

  // Add Joyride Site Tour
  //--------------------------
  //$form['#attached']['libraries_load'][] = array('joyride');

  // Add Shepherd Site Tour
  //--------------------------
  $crossmanage_path = drupal_get_path('module', 'cross_manage');
  $shepherd_path = libraries_get_path('shepherd');
  $form['#attached']['js'][] = $shepherd_path . '/shepherd.min.js';
  $form['#attached']['js'][] = $crossmanage_path . '/theme/js/CrossManage_F1form_ShepherdTour.js';
  $form['#attached']['css'][] = $shepherd_path .'/css/shepherd-theme-arrows.css';
  $form['#attached']['css'][] = array(
    'data' => $crossmanage_path . '/theme/css/crossmanage_shepherd_tour.css',
    'weight' => 10
  );

  // Add site tour link.
  //--------------------------
  $form['start_tour'] = array(
    '#type' => 'markup',
    '#markup' => theme('cross_manage_shepherd_start_tour_link')
  );

  // css which allows form item titles to float left of the text box to save space,
  // as well as for text boxes within the same item to align
  // ie: seed allocation fields
  $form['#attached']['css'][] = array(
    'type' => 'inline',
    'data' => '
      .form-item.form-type-textfield {
        text-align: right;
      }
      .form-item label {
        display:inline-block;
        width:55px;
        text-align:right;
      }
      .form-text {
        display:inline;
        width:auto;
      }
      td.type, th.type {
        width: 100px;
      }
      td.seed_num, th.seed_num {
        width: 90px;
        text-align: center;
      }
      td.seed_num .form-item.form-type-textfield {
        text-align: center;
      }
      td.seed_allocate, th.seed_allocate {
        width: 100px;
      }
    '
  );

  // PARENT OPTIONS
  // Currently includes all F1s
  // @TODO: Also should be able to select crosses resulting from the parents form?
  //        To pnum or not to pnum here?
  $parent_options = array('');
  foreach ($f1s as $k => $details) {
    $parent_options[] = $details->name;
  }
  $parent_options = array_combine($parent_options, $parent_options);

  $form['f1'] = array(
    '#type' => 'markup',
    '#title' => "F1's",
    // We denote this fieldset as a tree to ensure that the data is nested
    // when it makes it to the submit function.
    '#tree' => TRUE,
    '#theme' => 'cross_manage_crossingblock_f1'
  );

  // For each F1 we want to create a single row in the data entry table
  // allowing breeders to specify instructions per F1.
  // Titles are commented out to make things easier in the theme function
  // where these plain form fields will be rendered in a table layout.
  foreach ($f1s as $cross_num => $details) {

    $form['f1'][$cross_num]['cross_num'] = array(
      '#type' => 'item',
      //'#title' => 'Cross Number',
      '#markup' => $details->name,
    );

    // Same as cross_num, we also make it a hidden element to access it through
    // $form_state later.
    $form['f1'][$cross_num]['cross_name'] = array(
      '#type' => 'hidden',
      '#value' => $details->name,
    );

    $form['f1'][$cross_num]['type'] = array(
      '#type' => 'item',
      //'#title' => 'Type',
      '#markup' => $details->type_name,
    );

    $form['f1'][$cross_num]['pedigree'] = array(
      '#type' => 'item',
      //'#title' => 'Pedigree',
      '#markup' => $details->pedigree,
    );

    // Check to see if we know the number of seeds produced from the previous crossing block
    // for this F1. If not, display a textbox to allow breeders to enter seed amount if they
    // know it (ie. OPTIONAL).
    if(isset($details->num_seeds)) {
      $form['f1'][$cross_num]['seed_num'] = array(
        '#type' => 'item',
        //'#title' => 'Total Seed Produced',
        '#markup' => $details->num_seeds,
      );
    }
    else {
      $form['f1'][$cross_num]['seed_num'] = array(
        '#type' => 'textfield',
        '#attributes' => array('placeholder' => 'Unknown'),
        '#size' => '7',
      );
    }

    $form['f1'][$cross_num]['seed_allocate']['cross'] = array(
      '#type' => 'textfield',
      '#title' => t("Cross:"),
      '#size' => 3,
    );

    $form['f1'][$cross_num]['seed_allocate']['reserve'] = array(
      '#type' => 'textfield',
      '#title' => t("Reserve:"),
      '#size' => 3,
    );

    $form['f1'][$cross_num]['seed_allocate']['grow-out'] = array(
      '#type' => 'textfield',
      '#title' => t('Grow-out:'),
      '#size' => 3,
    );

    /** MARKERS
     * Add the ability to select markers to screen for each F1. Clicking on '+' would
     * bring up a popup window to allow breeders to browse/select markers.
     * @TODO: Implement marker selection
    /*
    $form['f1'][$cross_num]['marker_data']['markers2screen'] = array(
      '#type' => 'markup',
      //'#title' => 'Markers to Screen Already Selected',
      '#markup' => 'marker1, marker3, marker4'
        . '<img src="http://icons.iconarchive.com/icons/visualpharm/icons8-metro-style/256/Very-Basic-Plus-icon.png" height="16" weight="16" style="border:none;">'
        . ' '
        . '<img src="http://icons.iconarchive.com/icons/visualpharm/icons8-metro-style/256/Very-Basic-Minus-icon.png" height="16" weight="16" style="border:none;">'
    );
    */

    $form['f1'][$cross_num]['notes']['breeder_notes'] = array(
      '#type' => 'textarea',
      '#attributes' => array('placeholder' => 'Breeder Notes'),
      '#rows' => 1
    );

    $form['f1'][$cross_num]['notes']['instructions'] = array(
      '#type' => 'textarea',
      '#attributes' => array('placeholder' => 'Instructions'),
      '#rows' => 1
    );

    // Set up a wrapper to allow ajax to replace this item
    $cross_to_wrapper = 'cross-to-' . $cross_num;
    $form['f1'][$cross_num]['cross_to'] = array(
      '#type' => 'item',
      '#prefix' => '<div id="' . $cross_to_wrapper . '">',
      '#suffix' => '</div>',
      //'#title' => 'Cross To',
    );

    // Add a number of cross to dropdowns to allow breeders to specify what
    // parents they want to cross with the current F1
    // Ajax is used to ensure that there is always one more select box
    // than there are parents selected so that breeders can add more parents.

    // Determine how many select boxes we need.
    $num_crosses = (!empty($form_state['values']['f1'][$cross_num]['cross_to'])) ? sizeof($form_state['values']['f1'][$cross_num]['cross_to']) : 0;
    for ($j = 0; $j <= $num_crosses; $j++) {
      // Determine our default value for the next select box.
      if (isset($form_state['values']['f1'][$cross_num]['cross_to'][$j])) {
        // Default value is what has been previously entered
        $cross_to_default = $form_state['values']['f1'][$cross_num]['cross_to'][$j];
        // If the final box is already empty, don't iterate through the loop a final time
        // Otherwise, another empty box would be added.
        if (empty($form_state['values']['f1'][$cross_num]['cross_to'][$j]) AND ($j == ($num_crosses - 1))) {
          $num_crosses--;
        }
      }
      else { // This is a brand new select box, so make the default value empty.
        $cross_to_default = '';
      }

      // Then create a select box for each previously specified cross + one blank
      $form['f1'][$cross_num]['cross_to'][] = array(
        '#type' => 'select',
        '#options' => $parent_options,
        '#default_value' => $parent_options[$cross_to_default],
        '#ajax' => array(
          'callback' => 'ajax_f1_cross_to_callback',
          'wrapper' => $cross_to_wrapper,
        ),
      );
    }
  }

  // Should type here be button?
  $form['save-f1'] = array(
    '#type' => 'submit',
    '#value' => 'Save Progress',
  );

  $form['submit-f1'] = array(
    '#type' => 'submit',
    '#value' => 'Send to Field Lab',
  );

  return $form;
}

/**
 * Form validation for breeders to set crosses with F1s.
 * Ensures a cross cannot occur with itself, nor can 2+ identical crosses occur
 * BUG: Does not work as expected despite being the same code as for the Parents form
 * @TODO: Compare values rather than indices
 */
function cross_manage_crossingblock_set_cross_f1s_form_validate($form, &$form_state) {
  foreach ($form_state['values']['f1'] as $cross_key => $elements) {
    if (isset($form_state['values']['f1'][$cross_key]['cross_name'])) {
      $cross_name = $form_state['values']['f1'][$cross_key]['cross_name'];
      if (isset($form_state['values']['f1'][$cross_key]['cross_to'])) {
        // Check each selection to compare to the cross_num, then to other selections
        foreach ($form_state['values']['f1'][$cross_key]['cross_to'] as $k1 => $cross_to_num) {
          // If a selected cross-to matches the cross name, throw an error
          if ($cross_name == $cross_to_num) {
            form_set_error('f1][' . $cross_key . '][cross_to][' . $k1, 'Invalid cross: ' . $cross_name . ' to ' . $cross_to_num);
          }
          else {
            // If a selected cross-to is not empty, and is the same as a previously selected cross-to for that
            // cross number, throw an error.
            foreach ($form_state['values']['f1'][$cross_key]['cross_to'] as $k2 => $cross_option) {
              if (!empty($form_state['values']['f1'][$cross_key]['cross_to'][$k2]) AND ($cross_to_num == $cross_option) AND ($k1 != $k2)) {
                form_set_error('f1][' . $cross_key . '][cross_to][' . $k2, 'Duplicate cross: ' . $cross_name . ' to ' . $cross_option);
              //break 2; //breaking prevents two error messages from being seen but also highlights a single box from the duplicates
              }
            }
          }
        }
      }
    }
  }
}

/**
 * Form submission for breeders to submit F1 crosses to field lab.
 *
 * Implements hook_form_submit().
 */
function cross_manage_crossingblock_set_cross_f1s_form_submit($form, $form_state) {
  drupal_set_message('Successfully submitted F1s form to Field Lab');
}

/**
 * Callback for ajax-enabled select boxes
 * Retrieves the cross_num from the row that was "triggered" and returns the cross-to section
 * of the form for that row.
 */
function ajax_f1_cross_to_callback($form, $form_state) {
  $cross_num = $form_state['triggering_element']['#array_parents'][1];
  return $form['f1'][$cross_num]['cross_to'];
}
