<?php
/**
 * @file
 * Functionality related to the fourth step of the crossing block.
 */

/**
 * MOCK-UP Form allowing breeders to set the crosses to be made
 * for a given crossing block.
 *
 * Implements hook_form().
 */
function cross_manage_crossingblock_assign_cross_num_form($form, $form_state) {

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
    drupal_set_title(format_string('Report Crosses: @species, @season @year Crossing Block', array(
        '@species' => $species,
        '@year' => $cb_year,
        '@season' => $cb_season,
    )));

  }
  else {
    drupal_set_message('Unable to determine the current crossing block', 'error');
    return $form;
  }

  // Get a list of the cross properties for use later.
  $property_types = cross_manage_get_cb_property_type_ids('all');
  
  // If this form has not yet been submitted this session, check that all the 
  // planned crosses are associated with their stock if it exists.

  // Set Breadcrumb:
  //--------------------------
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Crossing Block Management', 'crossing-block');
  $breadcrumb[] = l($cb_season. ' ' . $cb_year, "crossing-block/$species/$cb_year/$cb_season/summary");
  $breadcrumb[] = l('Report Crosses', base_path() . request_uri()); // Link to current URL
  drupal_set_breadcrumb($breadcrumb);

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

  // Add Progress Bar
  //------------------
  $num_planned_crosses = db_query('
    SELECT count(*) FROM cross_manage_planned_crosses
    WHERE maternal_parent_id IN (SELECT cb_parent_id FROM cross_manage_parents WHERE cb_year=:year AND cb_season=:season AND cb_species=:genus)',
    array(':year' => $cb_year, ':season' => $cb_season, ':genus' => $species))->fetchField();
  $num_planned_complete = db_query('
    SELECT count(*) FROM cross_manage_planned_crosses
    WHERE cross_stock_id > 0
      AND maternal_parent_id IN (SELECT cb_parent_id FROM cross_manage_parents WHERE cb_year=:year AND cb_season=:season AND cb_species=:genus)',
    array(':year' => $cb_year, ':season' => $cb_season, ':genus' => $species))->fetchField();
  $num_unplanned = db_query('
  	SELECT count(*)
  	FROM chado.stock s
		LEFT JOIN chado.stockprop year ON year.stock_id=s.stock_id
		LEFT JOIN chado.stockprop season ON season.stock_id=s.stock_id
		LEFT JOIN chado.organism org ON org.organism_id=s.organism_id
		LEFT JOIN {cross_manage_planned_crosses} planned ON planned.cross_stock_id=s.stock_id
		WHERE
			year.type_id=3978 AND year.value=:year AND
			season.type_id=3977 AND season.value=:season AND
			org.genus=:genus AND
			planned.cross_stock_id IS NULL',
    array(':year' => $cb_year, ':season' => $cb_season, ':genus' => $species))->fetchField();

  $form['#attached']['js'][] = libraries_get_path('d3') . '/d3.min.js';
  $form['#attached']['js'][] = $crossmanage_path . '/theme/js/CrossManage_registerCrossesProgress.js';
  $form['#attached']['js'][] = array(
    'type' => 'setting',
    'data' => array('crossManage' => array('expected' => (int) $num_planned_crosses, 'correct' => (int) $num_planned_complete, 'incorrect' => (int) $num_unplanned))
  );
  $form['progress'] = array(
    '#type' => 'markup',
    '#markup' => '<div id="proportion-based-stacked-progressbar"></div>',
  );

  // First provide form for new crosses.
  //------------------------------------
  $max_crossnum = cross_manange_get_current_cross_number($species);

  $form['crosses'] = array(
    '#type' => 'markup',
    //'#title' => 'Register New Cross',
    '#tree' => TRUE,
    '#theme' => 'cross_manage_crossingblock_register_crosses',
  );


  // Retrieve the most recent crosses.
  $crosses = chado_query("
    SELECT s.stock_id, s.name as cross_num, cvt.name as type_name, org.genus||' '||org.species as organism, cs.nid,
      mat.subject_id as maternal_parent_id, pat.subject_id as paternal_parent_id
    FROM {stock} s
    LEFT JOIN public.chado_stock cs ON cs.stock_id=s.stock_id
    LEFT JOIN {cvterm} cvt ON cvt.cvterm_id=s.type_id
    LEFT JOIN {organism} org ON org.organism_id=s.organism_id
    LEFT JOIN {stock_relationship} mat ON mat.object_id=s.stock_id AND mat.type_id=3632
    LEFT JOIN {stock_relationship} pat ON pat.object_id=s.stock_id AND pat.type_id=3633
    WHERE
      org.genus=:cb_genus AND
      s.stock_id IN (
        SELECT year.stock_id
        FROM {stockprop} year, {stockprop} season
        WHERE
          year.type_id=3978 AND year.value=:cb_year AND
          season.type_id=3977 AND season.value=:cb_season AND
          year.stock_id=season.stock_id
      )
    ORDER BY s.name DESC
    LIMIT 10",
    array(
      ':cb_year' => $cb_year,
      ':cb_season' => $cb_season,
      ':cb_genus' => $species,
    )
  )->fetchAll();

  $num_rows = sizeof($crosses);

  $link_options = array(
    'attributes' => array( 'target' => '_blank' )
  );
  for ($i=-1; $i < $num_rows; $i++) {

    // -- First determine where we are in the table.
    if ($i == -1) {
      $first = TRUE;
      $cross = NULL;
      $key = 0;
    }
    else {
      $first = FALSE;
      $cross = $crosses[$i];
      $key = $cross->stock_id;
    }

    // -- Retrieve the properties/characteristics for the current cross.
    if (!$first) {

      $data = chado_query('SELECT type_id, value FROM {stockprop} WHERE type_id IN(:types) AND stock_id=:id',
        array(':types' => $property_types, ':id' => $cross->stock_id));
      $properties = array();
      foreach ($data as $p) {
        $properties[$p->type_id] = $p->value;
      }
    }

    // -- Cross Number.
    if ($first) {
      $form['crosses'][$key]['cross_name'] = array(
        '#type' => 'hidden' ,
        '#value' => $max_crossnum,
      );

      $plus_icon = 'data:image/png;base64,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';
      $form['crosses'][$key]['cross_num'] = array(
        '#type' => 'item' ,
        //'#prefix' => '<img class="add-new-icon" src="'.$plus_icon.'"/>',
        //'#title' => 'Cross Number',
        '#markup' => $max_crossnum,
      );
    }
    else {
      $cross_title = $cross->cross_num;
      if ($cross->nid) {
        $cross_title = l($cross_title, 'node/'.$cross->nid, $link_options);
      }
      $form['crosses'][$key]['cross_num'] = array(
        '#type' => 'item' ,
        //'#title' => 'Cross Number',
        '#markup' => $cross_title,
      );
    }

    // -- Type of Cross.
    if ($first) {
      $form['crosses'][$key]['type'] = array(
        '#type' => 'select' ,
        //'#title' => 'Type:',
        '#options' => cross_manage_get_cross_type_options(),
      );
    }
    else {
      $form['crosses'][$key]['type'] = array(
        '#type' => 'item' ,
        '#title' => 'Type:',
        '#markup' => $cross->type_name,
      );
    }

    // -- Species of Cross.
    // @TODO: smart choosing of default based on parents?
    if ($first) {
      $form['crosses'][$key]['species'] = array(
        '#type' => 'select' ,
        //'#title' => 'Species:',
        '#options' => cross_manage_get_species_options($species),
      );
    }
    else {
      $form['crosses'][$key]['species'] = array(
        '#type' => 'item' ,
        '#title' => 'Species:',
        '#markup' => $cross->organism,
      );
    }

    // -- Whether the cross ended up being a selfing of the Maternal Parent.
    if ($first) {
      $form['crosses'][$key]['selfed'] = array(
        '#type' => 'checkbox',
        '#title' => 'Self-pollinated?',
      );
    }
    else {
      $form['crosses'][$key]['selfed'] = array(
        '#type' => 'item',
        '#title' => 'Self-pollinated?',
        '#markup' => (isset($properties[ $property_types['selfed'] ])) ? 'Yes' : 'No',
      );
      if (!isset($properties[ $property_types['selfed'] ])) {
        $form['crosses'][$key]['selfed']['#prefix'] = '<span class="unknown-data">';
        $form['crosses'][$key]['selfed']['#suffix'] = '</span>';
      }
    }

    // -- Number of Seeds produced by the cross.
    if ($first) {
      $form['crosses'][$key]['num_seeds'] = array(
        '#type' => 'textfield',
        '#title' => 'Seed Count',
        '#size' => 3,
      );
    }
    else {
      $form['crosses'][$key]['num_seeds'] = array(
        '#type' => 'item',
        '#title' => 'Seed Count:',
        '#markup' => (isset($properties[ $property_types['num_seeds'] ])) ? $properties[ $property_types['num_seeds'] ] : 'Uknown',
      );
      if (!isset($properties[ $property_types['num_seeds'] ])) {
        $form['crosses'][$key]['num_seeds']['#prefix'] = '<span class="unknown-data">';
        $form['crosses'][$key]['num_seeds']['#suffix'] = '</span>';
      }
    }


    // Maternal Parent.
    $mat_icon = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAAAXNSR0IArs4c6QAAAAlwSFlzAAAPYQAAD2EBqD+naQAAAVlpVFh0WE1MOmNvbS5hZG9iZS54bXAAAAAAADx4OnhtcG1ldGEgeG1sbnM6eD0iYWRvYmU6bnM6bWV0YS8iIHg6eG1wdGs9IlhNUCBDb3JlIDUuNC4wIj4KICAgPHJkZjpSREYgeG1sbnM6cmRmPSJodHRwOi8vd3d3LnczLm9yZy8xOTk5LzAyLzIyLXJkZi1zeW50YXgtbnMjIj4KICAgICAgPHJkZjpEZXNjcmlwdGlvbiByZGY6YWJvdXQ9IiIKICAgICAgICAgICAgeG1sbnM6dGlmZj0iaHR0cDovL25zLmFkb2JlLmNvbS90aWZmLzEuMC8iPgogICAgICAgICA8dGlmZjpPcmllbnRhdGlvbj4xPC90aWZmOk9yaWVudGF0aW9uPgogICAgICA8L3JkZjpEZXNjcmlwdGlvbj4KICAgPC9yZGY6UkRGPgo8L3g6eG1wbWV0YT4KTMInWQAAAedJREFUOBGtlD1LA0EQhu8uOVEbPytjYxHtRBArO2sRwU6xtbBSxE7wN/gLLAXBThQCQQloZylioYWoiKDgBxokd+czl9ljyd3FxoE3Ozsz77uzt7txnHwr5Kec3JybQxJCADoLhcJsFEWT4oPrMAyPGO+A4Ub4bS1evVgsTnued4Vg1IIP4muqYERzBWMx3/cnEKmrUIOxBo7Bi8YiRNdV5e/tQzoVIqR7Op2xli8Rr6honfhoO1Gz0jiEHyUtWYSi+kMs9Cx5xg2NmZzjaUCG+HtQOILvg9cgCKqSUGswSvzRdd1ziTGWZcSSg7EF4wxF77HjOB2MIiAm3du13RLk9E1t5uEYQj9bedAtbQnRNg5sity35MGc5pIt27XixwkEN5UQ4otoCQwSm2d+ozn7c2R2KIJJAtKuEqWTN4SezBz/ktphIWDmMJuzjF+zdQeBRXBmCd0itg2nR3l/ihl9u3AAwU8VXTYFjHaNFU67psMyInugCuJ7SXcX+IdgQWmmNq1iReKrAmkF2O9YnqCZV7Q+1WXWccsFdrjU+yKAjXE3Vwl1gQPmNf5xTqQGSy50c5r/m5w2JX1WZ+YpCtOuSZRSLSeZ5suQfJ0Oe8EXXe8wl9chOwvBv1hmZ0a5bVKLWmvafrdf1iZ39Qoi4DsAAAAASUVORK5CYII=";
    if ($first) {
      $form['crosses'][$key]['maternal_parent'] = array(
        '#type' => 'textfield',
        //'#title' => 'Maternal Parent',
        '#attributes' => array('placeholder' => 'Maternal Parent'),
        //'#default_value' => $mat,
        '#size' => 30,
        '#autocomplete_path' => "tripal_ajax/cross_manage/parent_name/$species/$cb_year/$cb_season",
        '#prefix' => '<span class="existing-parent maternal"><img class="parental-icon maternal" src="' . $mat_icon .'"/>',
        '#suffix' => '</span>'
      );
    }
    else {
      $parent = db_query(
        'SELECT s.name, s.uniquename, cs.nid FROM chado.stock s LEFT JOIN {chado_stock} cs ON cs.stock_id=s.stock_id WHERE s.stock_id=:stock_id',
        array('stock_id' => $cross->maternal_parent_id)
      )->fetchObject();

      $mat = (is_object($parent)) ? $parent->name : '<i>Uknown</i>';
      $mat = (is_object($parent) AND $parent->nid) ? l($mat, 'node/'.$parent->nid, $link_options) : $mat;

      $form['crosses'][$key]['maternal_parent'] = array(
        '#type' => 'item',
        //'#title' => 'Maternal Parent',
        '#prefix' => '<span class="existing-parent maternal"><img class="parental-icon maternal" src="' . $mat_icon .'"/>',
        '#markup' => $mat,
        '#suffix' => '</span>'
      );
    }

    // -- Paternal Parent.
    $pat_icon = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAAAXNSR0IArs4c6QAAAAlwSFlzAAAPYQAAD2EBqD+naQAAAVlpVFh0WE1MOmNvbS5hZG9iZS54bXAAAAAAADx4OnhtcG1ldGEgeG1sbnM6eD0iYWRvYmU6bnM6bWV0YS8iIHg6eG1wdGs9IlhNUCBDb3JlIDUuNC4wIj4KICAgPHJkZjpSREYgeG1sbnM6cmRmPSJodHRwOi8vd3d3LnczLm9yZy8xOTk5LzAyLzIyLXJkZi1zeW50YXgtbnMjIj4KICAgICAgPHJkZjpEZXNjcmlwdGlvbiByZGY6YWJvdXQ9IiIKICAgICAgICAgICAgeG1sbnM6dGlmZj0iaHR0cDovL25zLmFkb2JlLmNvbS90aWZmLzEuMC8iPgogICAgICAgICA8dGlmZjpPcmllbnRhdGlvbj4xPC90aWZmOk9yaWVudGF0aW9uPgogICAgICA8L3JkZjpEZXNjcmlwdGlvbj4KICAgPC9yZGY6UkRGPgo8L3g6eG1wbWV0YT4KTMInWQAAAilJREFUOBGVlD1PFFEUhmd2FhBsDEGExoSYaGEB0YTEkpjQYaH+AkOMP8JGOxNre6EyFsSe0kYTkWhiQ234ULRV3N3xeS73jsOwi3CSd88597znnXPv3Nk8O5vl0Fug7NPW67N24pJi/7P8NCRFnMoJpoqieIy/CJzSfv1It9t9gt8EfU1iGxSxapwhtgzKAViSE4gGNVOkCzq1NfOMKVZardYw4ZU8zx/hh1wG58gP8McEk5jT3KV+D8wAJ94Fq4i+oPaSeAxoimpyjljanue0PmBbJRN+SzXiT2DDvN1uL6rmYWt6Rz8PYR1/GwQry/I7wdeYZmxtwpj1rV6vt0Du5JXVBTPEnkK4bpWGPdwDmmbZ5izrC+TvrWnkX3D78N6Cj51Op3rDSfQSgntxO78g37SxYcNwNiPHl3ajUc8VC4KcwRxP9X5pr8EH4Bv1bD3wUXAA5zleKxCfPwzDy5VTpunc4ngsGrsdzcvs2Xp5w7VAZCuu47IL/mBBzKAS5Mk/XdCIwzkSWndCvdNmnOnVmJumniPfdhKdrJ3hb47glh0NG4PzOZ7hH2pzsZ6uXEUPXwzkZ5Hs57UPHsK4DKa8Z9TDnYucV7E7DVSJGaTFURrrTQr/QGgnioTvmHybnumokHpj+s+lscdpXqsLNOJ3tFyLbamnUvHt1E1C+CNA5A7xfTAD5O2AN1zyVbxWcQ/Twb9uofmgJvvYZE1Cv9wXVT8fY9dOfNhfCVCfK9b1oGcAAAAASUVORK5CYII=";
    if ($first) {
      $form['crosses'][$key]['paternal_parent'] = array(
        '#type' => 'textfield',
        //'#title' => 'Paternal Parent',
        '#attributes' => array('placeholder' => 'Paternal Parent'),
        //'#default_value' => $pat,
        '#size' => 30,
        '#autocomplete_path' => "tripal_ajax/cross_manage/parent_name/$species/$cb_year/$cb_season",
        '#prefix' => '<span class="existing-parent paternal"><img class="parental-icon paternal" src="' . $pat_icon .'"/>',
        '#suffix' => '</span>'
      );
    }
    else {
      $parent = db_query(
        'SELECT s.name, s.uniquename, cs.nid FROM chado.stock s LEFT JOIN {chado_stock} cs ON cs.stock_id=s.stock_id WHERE s.stock_id=:stock_id',
        array('stock_id' => $cross->paternal_parent_id)
      )->fetchObject();

      $pat = (is_object($parent)) ? $parent->name : '<i>Uknown</i>';
      $pat = (is_object($parent) AND $parent->nid) ? l($pat, 'node/'.$parent->nid, $link_options) : $pat;

      $form['crosses'][$key]['paternal_parent'] = array(
        '#type' => 'item',
        //'#title' => 'Paternal Parent',
        '#prefix' => '<span class="existing-parent paternal"><img class="parental-icon paternal" src="' . $pat_icon .'"/>',
        '#markup' => $pat,
        '#suffix' => '</span>'
      );
    }


    $form['crosses'][$key]['data'] = array(
      '#type' => 'markup',
      '#tree' => TRUE,
    );

    // -- Cotyledon Colour.
    if ($first) {
      $form['crosses'][$key]['data']['cotyledon_colour'] = array(
        '#type' => 'textfield',
        //'#title' => 'Cotyledon Colour',
        '#attributes' => array('placeholder' => 'Cotyledon Colour'),
        '#size' => 17,
      );
    }
    else {
      $form['crosses'][$key]['data']['cotyledon_colour'] = array(
        '#type' => 'item',
        '#title' => 'Cotyledon:',
        '#markup' => (isset($properties[ $property_types['cotyledon_colour'] ])) ? $properties[ $property_types['cotyledon_colour'] ] : 'Uknown',
      );
      if (!isset($properties[ $property_types['cotyledon_colour'] ])) {
        $form['crosses'][$key]['data']['cotyledon_colour']['#prefix'] = '<span class="unknown-data">';
        $form['crosses'][$key]['data']['cotyledon_colour']['#suffix'] = '</span>';
      }
    }

    // -- Seed Coat Colour.
    if ($first) {
      $form['crosses'][$key]['data']['seedcoat_colour'] = array(
        '#type' => 'textfield',
        //'#title' => 'Seedcoat Colour',
        '#attributes' => array('placeholder' => 'Seed coat Colour'),
        '#size' => 17,
      );
    }
    else {
      $form['crosses'][$key]['data']['seedcoat_colour'] = array(
        '#type' => 'item',
        '#title' => 'Seed Coat:',
        '#markup' => (isset($properties[ $property_types['seedcoat_colour'] ])) ? $properties[ $property_types['seedcoat_colour'] ] : 'Uknown',
      );
      if (!isset($properties[ $property_types['seedcoat_colour'] ])) {
        $form['crosses'][$key]['data']['seedcoat_colour']['#prefix'] = '<span class="unknown-data">';
        $form['crosses'][$key]['data']['seedcoat_colour']['#suffix'] = '</span>';
      }
    }

    // -- Notes about the cross.
    if ($first) {
      $form['crosses'][$key]['notes'] = array(
        '#type' => 'textarea',
        '#attributes' => array('placeholder' => 'Notes'),
        '#rows' => 2,
        //'#suffix' => '<img class="add-new-icon" src="'.$plus_icon.'"/>',
      );
    }
    else {
      $form['crosses'][$key]['notes'] = array(
        '#type' => 'item',
        '#markup' => (isset($properties[ $property_types['notes'] ])) ? $properties[ $property_types['notes'] ] : '',
      );
    }

    // Action Button.
    // -- Seed Coat Colour.
    if ($first) {
      $form['crosses'][$key]['action'] = array(
        '#type' => 'image_button',
        '#value' => 'Add Cross',
        //'#src' => $plus_icon,
        '#src' => drupal_get_path('module', 'cross_manage') . '/theme/images/add-button.png',
        '#prefix' => '<span class="cross-parents add-more image-button green-button dark-well">',
        '#suffix' => '</span>',
      );
    }
    elseif ($cross->nid) {

      $img = theme_image(array(
        'path' => drupal_get_path('module','cross_manage').'/theme/images/open-new-page-navy.png',
        'title' => 'Edit Cross in New Page',
        'alt' => 'Edit Cross',
      'attributes' => array('class' => 'new-page-icon')
      ));
      $form['crosses'][$key]['action'] = array(
        '#type' => 'markup',
        '#markup' =>  l($img . 'edit', 'node/'.$cross->nid.'/edit', array('html' => TRUE, 'attributes' => array('target' => '_blank'))),
      );
    }
    else {
      $form['crosses'][$key]['action'] = array(
        '#type' => 'markup',
        '#markup' =>  ' ',
      );
    }

  }

  $form['submit-cb'] = array(
    '#type' => 'submit',
    '#value' => 'Finalize Crossing Block',
  );

  return $form;
}

/**
 * Form allowing breeders to set the crosses to be made
 * for a given crossing block.
 *
 * Implements hook_form_validate().
 */
function cross_manage_crossingblock_assign_cross_num_form_validate($form, $form_state) {

  if (isset($form_state['clicked_button']) AND $form_state['clicked_button']['#value'] == 'Add Cross') {

    // Variables to make validation easier.
    $new_cross = $form_state['values']['crosses'][0];
    $pnums = array();

    // VALIDATE: Ensure requred fields are filled in.
    // Doing the validation here rather than #required in the form b/c the errors are not
    // showing up for non-titled fields...
    if (empty($new_cross['maternal_parent'])) {
      form_set_error('crosses][0][maternal_parent', 'The <em>Maternal Parent</em> is required to register a new cross.');
    }
    if (empty($new_cross['paternal_parent'])) {
      form_set_error('crosses][0][paternal_parent', 'The <em>Paternal Parent</em> is required to register a new cross.');
    }
    if (empty($new_cross['num_seeds']) AND $new_cross['num_seeds'] !== '0') {
      form_set_error('crosses][0][data][num_seeds', 'The <em>Number of Seeds</em> is required to register a new cross.');
    }

    // VALIDATE: Parents are in the parents table for this crossing block.
    $parents = array(
      array('title' => 'Maternal Parent', 'field' => 'maternal_parent'),
      array('title' => 'Paternal Parent', 'field' => 'paternal_parent'),
    );
    foreach ($parents as $p) {
      // -- 1st check they used the drop-down.
      if (preg_match('/^P(\d+): (.*)$/',$new_cross[ $p['field'] ], $matches)) {
        // -- Now check the parent actually exists.
        $present = db_query("
          SELECT true
          FROM cross_manage_parents p
          LEFT JOIN chado.stock s ON s.stock_id=p.stock_id
          WHERE p.cb_year=:year AND p.cb_season=:season AND p.cb_species=:genus AND p.parent_number=:pnum AND s.name=:name",
          array(
            ':year' => $form_state['build_info']['args'][1],
            ':season' => $form_state['build_info']['args'][2],
            ':genus' => $form_state['build_info']['args'][0],
            ':pnum' => $matches[1],
            ':name' => $matches[2],
          ))->fetchField();
          if (!$present) {
            form_set_error('crosses][0][' . $p['field'], 'The <em>' . $p['title'] . '</em> you entered is not a parent of this crossing block. Please add it as a parent first.');
          }
          else {
            // Keep track of the parent number so we don't need to extract it 2X.
            $pnums[$p['field']] = $matches[1];
          }
      }
      else {
        form_set_error('crosses][0][' . $p['field'], 'Please select the <em>' . $p['title'] . '</em> from the autocomplete drop-down.');
      }
    }

    // VALIDATE: Seed count is a positive whole number.
    // Check that it's not 0
    if ($new_cross['num_seeds'] === '0') {
      form_set_error('crosses][0][data][num_seeds', 'It\'s assumed that all crosses registered should have <em>at least one seed</em>; failed crosses should not be registered.');
    }
    // Check 1) it's a number, 2) it's positive, and 3) it's a whole number.
    elseif (!is_numeric($new_cross['num_seeds']) || $new_cross['num_seeds'] < 1 || $new_cross['num_seeds'] != round($new_cross['num_seeds'])) {
      form_set_error('crosses][0][data][num_seeds', 'The <em>Number of Seeds</em> must be a positive whole number.');
    }

    // VALIDATE: Seed color characteristics are actually colors?
    $unacceptable_modifiers = array('marbled','mottled','spotted','dotted','speckled','stained','heavy','solid','dusty', 'like');
    $color_fields = array(
      array('title' => 'Cotyledon Color', 'field' => 'cotyledon_colour', 'plant_part' => 'cotyledon'),
      array('title' => 'Seed Coat Color', 'field' => 'seedcoat_colour', 'plant_part' => 'seed coat'),
    );
    foreach ($color_fields as $c) {
      str_replace($unacceptable_modifiers, '', $new_cross['data'][ $c['field'] ], $count);
      if ($count) {
        form_set_error('crosses][0][data][' . $c['field'], 'Please enter the main colour of the '.$c['plant_part'].' without any modifiers (marbled, mottled, spotted, stained).');
      }
    }

    // VALIDATE: Check that the selected species makes sence based on the species of the parents?
    if (isset($pnums['maternal_parent']) AND isset($pnums['paternal_parent'])) {
      $maternal = cross_manage_get_parent(array(
        'parent_number' => $pnums['maternal_parent'],
        'cb_year' => $form_state['build_info']['args'][1],
        'cb_season' => $form_state['build_info']['args'][2],
        'cb_species' => $form_state['build_info']['args'][0]
      ));

      $paternal = cross_manage_get_parent(array(
        'parent_number' => $pnums['paternal_parent'],
        'cb_year' => $form_state['build_info']['args'][1],
        'cb_season' => $form_state['build_info']['args'][2],
        'cb_species' => $form_state['build_info']['args'][0]
      ));

      $maternal_organism_id = $maternal->stock->organism_id->organism_id;
      $paternal_organism_id = $paternal->stock->organism_id->organism_id;
      if ($new_cross['species'] != $maternal_organism_id AND $new_cross['species'] != $paternal_organism_id) {
        if ($maternal_organism_id == $paternal_organism_id) {
          $species = $maternal->stock->organism_id->genus .' '. $maternal->stock->organism_id->species;
          form_set_error('crosses][0][species', 'Since both parents are <em>'.$species.'</em>, the resulting cross species should also be <em>'.$species.'</em>.');
        }
        else {
          $mat_species = $maternal->stock->organism_id->genus .' '. $maternal->stock->organism_id->species;
          $pat_species = $paternal->stock->organism_id->genus .' '. $paternal->stock->organism_id->species;
          form_set_error('crosses][0][species', 'The current cross is interspecific. In this case you should choose the most represented species from the parents (<em>'.$mat_species.'</em> OR <em>'.$pat_species.'</em>)');
        }
      }
    }
  }

}

/**
 * Form allowing breeders to set the crosses to be made
 * for a given crossing block.
 *
 * Implements hook_form_submit().
 */
function cross_manage_crossingblock_assign_cross_num_form_submit($form, $form_state) {

  if (isset($form_state['clicked_button']) AND $form_state['clicked_button']['#value'] == 'Add Cross') {
    // Add the new cross.
    $stock = array(
      'name' => $form_state['values']['crosses'][0]['cross_name'],
      'uniquename' => uniqid(),
      'organism_id' => $form_state['values']['crosses'][0]['species'],
      'type_id' => $form_state['values']['crosses'][0]['type'],
    );
    $stock = chado_insert_record('stock', $stock);

    // Update the stock with the new uniquename
    // We can't set this initially since it depends on the stock_id
    // which is only available after the stock is created
    $prefix = variable_get('germplasm_prefix', 'GERM:');
    $stock['uniquename'] = $prefix . $stock['stock_id'];
    chado_update_record('stock', array('stock_id' => $stock['stock_id']), $stock);

    // Generate the primary dbxref
    $db_id = variable_get('tripal_stock_main_dbxref_db', 0);
    if ($db_id > 0) {
      $dbxref = tripal_insert_dbxref(array(
        'db_id' => $db_id,
        'accession' => $stock['uniquename'],
      ));

      // Update the stock to point to the new dbxref
      if (is_object($dbxref)) {
        $stock['dbxref_id'] = $dbxref->dbxref_id;
        chado_update_record('stock', array('stock_id' => $stock['stock_id']), $stock);
      }
    }

    $type_ids = cross_manage_get_cb_property_type_ids('all');

    // Assign it to the current crossing block.
    // -- Year.
    $record = array('table' => 'stock', 'id' => $stock['stock_id']);
    $property = array(
        'type_id' => $type_ids['year'],
        'value' => $form_state['build_info']['args'][1],
        'rank' => 0
      );
    $options = array('update_if_present' => TRUE);
    chado_insert_property($record, $property, $options);

    // -- Season.
    $record = array('table' => 'stock', 'id' => $stock['stock_id']);
    $property = array(
        'type_id' => $type_ids['season'],
        'value' => $form_state['build_info']['args'][2],
        'rank' => 0
      );
    $options = array('update_if_present' => TRUE);
    chado_insert_property($record, $property, $options);

    // Save the core properties (ie: number of seeds and selfing?).
    foreach (array('num_seeds', 'selfed') as $key) {
      $value = $form_state['values']['crosses'][0][$key];
      if (!empty($value)) {
        $record = array('table' => 'stock', 'id' => $stock['stock_id']);
        $property = array(
            'type_id' => $type_ids[$key],
            'value' => $value,
            'rank' => 0
          );
        $options = array('update_if_present' => TRUE);
        chado_insert_property($record, $property, $options);
      }
    }

    // Save each parent.
    // -- Maternal Parent.
    $parent_rel_type_id = variable_get('germplasm_mparent_rel_type_id', NULL);
    if (preg_match('/[PF](\d+): (.*)/', $form_state['values']['crosses'][0]['maternal_parent'], $matches)) {
      $parent = db_query("
        SELECT s.stock_id, p.cb_parent_id
        FROM {cross_manage_parents} p
        LEFT JOIN chado.stock s ON s.stock_id=p.stock_id
        WHERE p.cb_species=:species AND p.cb_year=:year AND p.cb_season=:season
          AND s.name=:name AND (p.parent_number=:pnum OR p.parent_number='F1')",
        array(
          ':species' => $form_state['build_info']['args'][0],
          ':year' => $form_state['build_info']['args'][1],
          ':season' => $form_state['build_info']['args'][2],
          ':name' => $matches[2],
          ':pnum' => $matches[1]
      ))->fetchObject();

      $rel = array(
        'subject_id' => $parent->stock_id,
        'type_id' => $parent_rel_type_id,
        'object_id' => $stock['stock_id'],
        'rank' => 0
      );
      $exists = chado_select_record('stock_relationship', array('*'), $rel);
      if (!$exists) {
        chado_insert_record('stock_relationship', $rel);
      }
      
      // Save the cross manage parent id to associate this stock with the originally
      // requested cross.
      $maternal_cb_parent_id = $parent->cb_parent_id;
    }

    // -- Paternal Parent.
    $parent_rel_type_id = variable_get('germplasm_pparent_rel_type_id', NULL);
    if (preg_match('/[PF](\d+): (.*)/', $form_state['values']['crosses'][0]['paternal_parent'], $matches)) {
      $parent = db_query("
        SELECT s.stock_id, p.cb_parent_id
        FROM {cross_manage_parents} p
        LEFT JOIN chado.stock s ON s.stock_id=p.stock_id
        WHERE p.cb_species=:species AND p.cb_year=:year AND p.cb_season=:season
          AND s.name=:name AND (p.parent_number=:pnum OR p.parent_number='F1')",
        array(
          ':species' => $form_state['build_info']['args'][0],
          ':year' => $form_state['build_info']['args'][1],
          ':season' => $form_state['build_info']['args'][2],
          ':name' => $matches[2],
          ':pnum' => $matches[1]
      ))->fetchObject();

      $rel = array(
        'subject_id' => $parent->stock_id,
        'type_id' => $parent_rel_type_id,
        'object_id' => $stock['stock_id'],
        'rank' => 0
      );
      $exists = chado_select_record('stock_relationship', array('*'), $rel);
      if (!$exists) {
        chado_insert_record('stock_relationship', $rel);
      }
      
      // Save the cross manage parent id to associate this stock with the originally
      // requested cross.
      $paternal_cb_parent_id = $parent->cb_parent_id;
    }

		// Associate this stock with the original requested cross (if it exists).
		if ($maternal_cb_parent_id AND $paternal_cb_parent_id) {
			$cb_cross = array(
				'maternal_parent_id' => $maternal_cb_parent_id,
				'paternal_parent_id' => $paternal_cb_parent_id,
				'cross_stock_id' => $stock['stock_id']
			);
			$success = drupal_write_record('cross_manage_planned_crosses', $cb_cross, array('maternal_parent_id', 'paternal_parent_id'));
		}
		
    // Add characteristics.
    foreach($form_state['values']['crosses'][0]['data'] as $key => $value) {
      if (!empty($value)) {
        $record = array('table' => 'stock', 'id' => $stock['stock_id']);
        $property = array(
            'type_id' => $type_ids[$key],
            'value' => $value,
            'rank' => 0
          );
        $options = array('update_if_present' => TRUE);
        chado_insert_property($record, $property, $options);
      }
    }

    // Save Notes.
    if (!empty($form_state['values']['crosses'][0]['notes'])) {
      $record = array('table' => 'stock', 'id' => $stock['stock_id']);
      $property = array(
          'type_id' => $type_ids['notes'],
          'value' => $form_state['values']['crosses'][0]['notes'],
          'rank' => 0
        );
      $options = array('update_if_present' => TRUE);
      chado_insert_property($record, $property, $options);
    }

    // Finally create a node associated with this stock.
    // This is essentially sync'ing for a single stock.
    // -- Create generic new node.
    global $user;
    $new_node = new stdClass();
    $new_node->type = 'chado_stock';
    $new_node->uid = $user->uid;
    $new_node->stock_id = $stock['stock_id'];
    $new_node->stock = chado_generate_var('stock', array('stock_id' => $stock['stock_id']));
    $new_node->language = LANGUAGE_NONE;

    // -- Validate New Node
    $form = array();
    $form_state = array();
    node_validate($new_node, $form, $form_state);

    // -- Save node as long as validation was successful.
    if (!form_get_errors()) {
      $node = node_submit($new_node);
      node_save($node);
    }
    else {
      throw new Exception(t("Failed to insert $base_table: %title", array('%title' => $new_node->title)));
    }
  } // End of add cross.

}
