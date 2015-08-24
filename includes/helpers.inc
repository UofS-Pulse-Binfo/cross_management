<?php
/**
 * @file
 * Contains helper functions related to cross management.
 */

/**
 * Retrieve the Parent/Stock details
 */
function cross_manage_get_parent($identifier) {
  // A list of possible identifiers.
  $possible_fields = array('cb_parent_id' => 'p','cb_year' => 'p','cb_season' => 'p','cb_species' => 'p', 'parent_number' => 'p','stock_id' => 'p','name' => 's', 'uniquename' => 's');
  
  $where = array();
  $args = array();
  foreach ($possible_fields as $field => $alias) {
    if (isset($identifier[$field])) {
      $where[] = $alias.'.'.$field.'=:'.$field;
      $args[':'.$field] = $identifier[$field];
    }
  }
  
  $parent = db_query('SELECT p.* FROM {cross_manage_parents} p LEFT JOIN chado.stock s ON s.stock_id=p.stock_id WHERE '.implode(' AND ',$where), $args)->fetchObject();

  $stock = chado_generate_var('stock', array('stock_id' => $parent->stock_id));
  $parent->stock = $stock;
  
  return $parent;
}

/**
 * Retrieve the stock_id based on the name.
 * Supported name locations include stock.name, stock.uniquename.
 */
function cross_manage_get_stock_by_name($name, $genus) {

  $result = chado_query('
    SELECT stock_id
    FROM {stock} s
    LEFT JOIN {organism} o ON o.organism_id=s.organism_id
    WHERE (s.name=:name OR s.uniquename=:name)
      AND o.genus=:genus',
    array(':name' => $name, ':genus' => $genus))->fetchAll();

  if (sizeof($result) == 1) {
    return $result[0]->stock_id;
  }
  return $result;
}

/**
 * Retrieve the current cross number for a given crop.
 *
 * @param $crop
 */
function cross_manange_get_current_cross_number($crop) {

  // Retrieve the current cross_number based on the crop.
  $cross_num = variable_get('cross_manage_current_crossnum_'.$crop, 99000);

  // Increment the cross number.
  $cross_num++;
  
  // Double check that this number has not already been used.
  
  
  // Set the new current cross number.
  variable_set('cross_manage_current_crossnum_'.$crop, $cross_num);
  
  return $cross_num;
}

/************************/
/* -- Retrieve Type_ids */

/**
 * Retrieve type_ids to specify stock crossing block.
 *
 * @param $type
 *   One of 'year', 'season' or 'all' to specify which types are needed.
 */
function cross_manage_get_cb_property_type_ids($type) {
  $type_names = array(
    'year' => 'crossingblock_year',
    'season' => 'crossingblock_season',
    'num_seeds' => 'number_seeds_produced',
    'cotyledon_colour' => 'cotyledon_colour',
    'seedcoat_colour' => 'seed_coat_colour',
    'selfed' => 'self_pollination'
  );
  
  if ($type == 'all') {
    $type_id = array();
    
    $results = chado_query('SELECT cvterm_id, name FROM {cvterm} WHERE name IN (:types)', array(':types' => $type_names));
    foreach ($results as $r) {
      $type_category = array_search($r->name, $type_names);
      $type_id[$type_category] = $r->cvterm_id;
    }
  }
  else {
    $type_id = 0;
    $results = chado_query('SELECT cvterm_id, name FROM {cvterm} WHERE name=:type', array(':type' => $type_names[$type]));
    foreach ($results as $r) {
      $type_id = $r->cvterm_id;
    }
  }
  
  return $type_id;
}

/********************/
/* -- Autocompletes */

/**
 * FORM AUTOCOMPLETE: Return stock name (no id).
 */
function cross_manage_stock_name_autocomplete($genus, $string) {
  $matches = array();

  $results = chado_query('
    SELECT s.name
    FROM {stock} s
    LEFT JOIN {organism} o ON o.organism_id=s.organism_id
    WHERE s.name~:string AND o.genus=:genus
    ORDER BY char_length(s.name) ASC, s.name ASC
    LIMIT 10',
    array(':genus' => $genus, ':string' => $string)
  );
  foreach ($results as $r) {
    $matches[$r->name] = $r->name;
  }

  drupal_json_output($matches);
}

/**
 * FORM AUTOCOMPLETE: Return parent number/name.
 */
function cross_manage_parent_name_autocomplete($genus, $year, $season, $string) {
  $matches = array();

  if (preg_match('/P(\d+)$/', $string, $regex_matches)) {
    $results = chado_query("
      SELECT s.name, p.parent_number
      FROM public.cross_manage_parents p
      LEFT JOIN {stock} s ON s.stock_id=p.stock_id
      WHERE p.parent_number~:string AND cb_species=:genus AND cb_year=:year AND cb_season=:season AND p.parent_number!='F1'
      ORDER BY char_length(s.name) ASC, s.name ASC
      LIMIT 10",
      array(':genus' => $genus, ':year' => $year, ':season' => $season, ':string' => $regex_matches[1])
    );
  }
  else {
    $results = chado_query('
      SELECT s.name, p.parent_number
      FROM public.cross_manage_parents p
      LEFT JOIN {stock} s ON s.stock_id=p.stock_id
      WHERE s.name~:string AND cb_species=:genus AND cb_year=:year AND cb_season=:season
      ORDER BY char_length(s.name) ASC, s.name ASC
      LIMIT 10',
      array(':genus' => $genus, ':year' => $year, ':season' => $season, ':string' => $string)
    );
  }
  foreach ($results as $r) {

    if ($r->parent_number == 'F1') {
      $to_show = 'F1: '.$r->name;
    }
    else {
      $to_show = 'P'.$r->parent_number.': '.$r->name;
    }

    $matches[$to_show] = $to_show;
  }

  drupal_json_output($matches);
}

/************************/
/* -- Options Retrieval */

/**
 * Retrieve parent (including F1) options for drop-downs.
 */
function cross_manage_get_parent_options($species, $year, $season) {
  $options = array(0 => '');

  $parents = db_query('
    SELECT p.cb_parent_id, s.name, p.parent_number
    FROM {cross_manage_parents} p
    LEFT JOIN chado.stock s ON s.stock_id=p.stock_id
    WHERE cb_species=:species AND cb_year=:year AND cb_season=:season
    ORDER BY s.name ASC',
    array(':species' => $species, ':year' => $year, ':season' => $season));
  foreach ($parents as $p) {
    if ($p->parent_number == 'F1') {
      $options[$p->cb_parent_id] = $p->name .' (F1)';
    } else {
      $options[$p->cb_parent_id] = $p->name .' (P'.$p->parent_number.')';
    }
  }

  return $options;
}

/**
 * Retrieve Cross Type Options.
 * This includes all cvterms with "cross" in the name.
 */
function cross_manage_get_cross_type_options() {
	$options = array();
	
	$results = chado_query('SELECT cvterm_id, name FROM {cvterm} WHERE name~:partial AND cv_id=:cv', array(':partial' => '[Cc]ross', ':cv' => 509));
	foreach ($results as $r) {
		if (preg_match('/(\S+) Cross/',$r->name,$matches)) {
			$options[$r->cvterm_id] = $matches[1];
		}
		else {
			$options[$r->cvterm_id] = $r->name;
		}
	}	
	
	uasort($options, 'cross_manage_sort_cross_type_string_logically');
	
	return $options;
}

/**
 * Retrive the organism.species options based on the Genus.
 */
function cross_manage_get_species_options($genus) {
	$options = array();
	
	$results = chado_query('SELECT organism_id, species FROM {organism} WHERE genus=:genus ORDER BY species ASC', array(':genus' => $genus));
	foreach ($results as $r) {
		$options[$r->organism_id] = $r->species;
	}
	
	return $options;
}

/*******************************/
/* -- Sorts, Formats & Filters */

/**
 * Custom sort function: Sort cross management parents numerically.
 *
 * NOTE: Cannot use regular numeric sort because the parent number is stored
 * as a string to allow use of A, B, C, etc.
 * NOTE: Cannot use natsort() because the parents array is an array of objects.
 */
function cross_manage_sort_parents_by_pnumber($a, $b) {
  return strnatcasecmp($a->parent_number, $b->parent_number);
}

/**
 * Custom sort function: Sort cross type names logically.
 *
 * For example, we want to ensure "Single" comes before "Double".
 */
function cross_manage_sort_cross_type_string_logically($a, $b) {	
	$expected_order = array('Cross', 'Single', 'Backcross', 'Double', 'Triple', 'Multiple');
	
	$A_order_key = array_search($a, $expected_order);
	$B_order_key = array_search($b, $expected_order);
  
  if ($A_order_key AND $B_order_key) {
  	return ($B_order_key < $A_order_key) ? 1 : -1;
  }
  elseif ($A_order_key) {
  	return -1;
  }
  else {
  	return 1;
  }
}

/**
 * Custom Callback: Keep elements with a value greater than 2.
 * @see array_filter().
 */
function cross_manage_filter_keep_2plus($val, $key) {
  return ($val >= 2) ? TRUE : FALSE;
}

/**
 * Format Number: Add ordinal suffix (ie: 3rd).
 */
function cross_manage_format_number_ordinal($number) {

  $ends = array('th','st','nd','rd','th','th','th','th','th','th');
  if ((($number % 100) >= 11) && (($number%100) <= 13)) {
    return $number. 'th';
  }
  else {
    return $number. $ends[$number % 10];
  }

}
