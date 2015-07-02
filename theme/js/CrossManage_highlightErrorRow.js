(function ($) {
  Drupal.behaviors.crossmanageParentsRowError = {
    attach: function (context, settings) {
      var errorElements = $(".error");
      var errorRows = errorElements.parents("tr");
      errorRows.addClass("error-row");
  }};
}(jQuery));
