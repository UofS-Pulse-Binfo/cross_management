/**
 * Highlight the entire row of a table when a drupal error message is rendered within
 * any of the cells. This is used to draw the attention of the user since otherwise
 * these messages are easy to overlook.
 *
 * Note: You end up with error messages within a table through rendering of drupal
 * messages on ajax.
 */

(function ($) {
  Drupal.behaviors.crossmanageParentsRowError = {
    attach: function (context, settings) {
      
      // First remove the .error-row class from any rows 
      // where there was previously an error.
      $("tr.error-row").removeClass("error-row");
      
      // Now add the class to any rows with a drupal set message in one of the cells.
      var errorElements = $(".error");
      var errorRows = errorElements.parents("tr");
      errorRows.addClass("error-row");
  }};
}(jQuery));
